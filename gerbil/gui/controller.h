#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model/image.h"
#include "model/labeling.h"
#include "model/illumination.h"
#include "model/falsecolor.h"
#include "model/graphsegmentation.h"
#include "model/ussegmentationmodel.h"
#include "mainwindow.h"
#include <background_task.h>
#include <background_task_queue.h>

#include <QObject>
#include <QMap>
#include <tbb/compat/thread>

class DockController;

class Controller : public QObject
{
	Q_OBJECT
public:
	explicit Controller(const std::string &filename, bool limited_mode);
	~Controller();

	MainWindow* mainWindow() { return window; }
	ImageModel* imageModel() { return &im; }
	FalseColorModel* falseColorModel() { return &fm; }
	IllumModel* illumModel() { return &illumm; }
	GraphSegmentationModel* graphSegmentationModel() { return &gsm; }
	UsSegmentationModel* usSegmentationModel() { return &um; }
	LabelingModel* labelingModel() {return &lm;}
signals:
	void nSpectralBandsChanged(int);
	// setGUIEnabled() part of the dock windows
	void requestEnableDocks(bool enable, TaskType tt);

public slots:
	/** requests (from GUI) */
	// change ROI, effectively spawning new image data, reusing cached ROI data
	void spawnROI(const cv::Rect &roi);
	// change ROI, spawn new image data, rebuild everything from scratch.
	void invalidateROI(const cv::Rect &roi);
	// change number of bands, spawning new image data
	void rescaleSpectrum(size_t bands);
	// change binnig to reflect, or not reflect, labeling
	void toggleLabels(bool toggle);

	// load a labeling from file
	void loadLabeling(const QString &filename = QString())
	{ lm.loadLabeling(filename); }
	// save a labeling to file
	void saveLabeling(const QString &filename = QString())
	{ lm.saveLabeling(filename); }

	/** notifies (from models, tasks) */
	// label colors added/changed
	void propagateLabelingChange(const cv::Mat1s &labels,
								 const QVector<QColor>& colors = QVector<QColor>(),
								 bool colorsChanged = false);

	/** Enable and disable GUI as indicated by enable flag.
	 *
	 * When disable is requested all GUI elements other than those involved
	 * with the task indicated by TaskType whill be disabled. This is to enable
	 * the user to trigger a new operation of the same TaskType before the
	 * previous one is finished.  In this case the model in charge should
	 * cancel the ongoing calculations and re-start with the new user input.
	 */
	void setGUIEnabled(bool enable, TaskType tt = TT_NONE);

	/** internal management (maybe make protected) */
	/* this function enqueues an empty task that will signal when all previous
	 * tasks are finished. the signal will trigger enableGUINow, and the
	 * GUI gets re-enabled at the right time.
	 * The roi argument is needed to specify if the other enqueued tasks are
	 * roi-bound. If somebody cancels all tasks with that roi, our task should
	 * be cancelled as-well, and re-enable not take place.
	 */
	void enableGUILater(bool withROI = false);
	// the other side of enableGUILater
	void enableGUINow(bool forreal);
	void disableGUI(TaskType tt = TT_NONE);

protected:
	// connect models with gui
	void initImage();
	void initFalseColor();
	void initIlluminant();
	void initGraphSegmentation();
	void initLabeling(cv::Rect dimensions);

	// create background thread that processes BackgroundTaskQueue
	void startQueue();
	// stop and delete thread
	// (we did not test consecutive start/stop of the queue)
	void stopQueue();

	void doSpawnROI(bool reuse, const cv::Rect &roi);

	// update ROI, or its contents
	void updateROI(bool reuse, cv::Rect roi = cv::Rect(), size_t bands = -1);

	// image model stores all multispectral image representations (IMG, GRAD, …)
	ImageModel im;

	/* false color model generates and stores RGB representations of
	 * multispectral data */
	FalseColorModel fm;

	IllumModel illumm;
	GraphSegmentationModel gsm;

	// labeling model stores pixel/label associations and label color codes
	LabelingModel lm;

#ifdef WITH_SEG_MEANSHIFT
	// unsupervised segmentation model
	UsSegmentationModel um;
#endif /* WITH_SEG_MEANSHIFT */

	// setup dock widgets and manage interaction with models
	DockController *dc;

	// main window (or gui slave)
	MainWindow *window;

	BackgroundTaskQueue queue;
	std::thread *queuethread;

	/* A map of BackgroundTasks (QObject) to representations so that we know
	 * what representation a signaling task was working on */
	QMap<QObject*, representation> taskmap;

	// True if GUI needs to be notified about changed number of spectral bands.
	// See signal nSpectralBandsChanged(int).
	bool spectralRescaleInProgress;
};

#endif // CONTROLLER_H
