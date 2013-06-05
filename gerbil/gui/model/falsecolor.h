#ifndef MODEL_FALSECOLOR_H
#define MODEL_FALSECOLOR_H

// for representations
#include "model/image.h"
#include <background_task_queue.h>
#include <commandrunner.h>
#include <rgb.h>
#include <multi_img.h>
#include <shared_data.h>

#include <QImage>
#include <QMap>
#include <QObject>

// QImages have implicit data sharing, so the returned objects act as a pointer, the data is not copied
// If the QImage is changed in the Model, this change is calculated on a copy,
// which is then redistributed by a loadComplete signal

// Each request sends a loadComplete signal to ALL connected slots.
// use QImage.cacheKey() to see if the image was changed

class FalseColorModelPayload;

class FalseColorModel : public QObject
{
	Q_OBJECT

public:
	enum coloring {
		CMF = 0,
		PCA = 1,
		SOM = 2,
		COLSIZE = 3
	};

	typedef FalseColorModelPayload payload;

	/* construct model without image data. Make sure to call setMultiImg()
	 * before doing any other operations with this object.
	 */
	FalseColorModel(BackgroundTaskQueue *queue);
	~FalseColorModel();

	// calls reset()
	void setMultiImg(representation::t repr, SharedMultiImgPtr img);

	// resets current true / false color representations
	// on the next request, the color images are recalculated with possibly new multi_img data
	// (CommandRunners are stopped by terminateTasksDeleteRunners())
	void reset();

public slots:
	// Img could be calculated in background, if the background task was started before!
	void calculateForeground(coloring type);
	void calculateBackground(coloring type);

signals:
	// Possibly check Image.cacheKey() to determine if the update is really neccessary
	void calculationComplete(FalseColorModel::coloring type, QImage img);
	void terminateRunners();

private:
	typedef QList<payload*> PayloadList;
	typedef QMap<coloring, payload*> PayloadMap;

	// creates the runner that is neccessary for calculating the false color representation of type
	// runners are deleted in terminatedTasksDeleteRunners (and therefore in reset() & ~FalseColor())
	void createRunner(coloring type);

	// terminates all (queue and commandrunner) tasks and waits until the terminate is complete
	void cancel();

	SharedMultiImgPtr shared_img;
	PayloadMap map;
	BackgroundTaskQueue *queue;
};

class FalseColorModelPayload : public QObject {
	Q_OBJECT

public:
	FalseColorModelPayload(representation::t repr, FalseColorModel::coloring type)
		: repr(repr), type(type), runner(0)
	{}

	virtual ~FalseColorModelPayload() {}

	representation::t repr;
	FalseColorModel::coloring type;
	CommandRunner *runner;
	QImage img;
	qimage_ptr calcImg;  // the background task swaps its result in this variable, in taskComplete, it is copied to img & cleared
	bool calcInProgress; // (if 2 widgets request the same coloring type before the calculation finished)

public slots:
	// connect the corresponding task's finishing with this slot
	void propagateFinishedQueueTask(bool success);
	void propagateRunnerSuccess(std::map<std::string, boost::any> output);

signals:
	void calculationComplete(FalseColorModel::coloring type, QImage img);
};
#endif // MODEL_FALSECOLOR_H
