#ifndef DOCKCONTROLLER_H
#define DOCKCONTROLLER_H

#include "../common/background_task.h"
#include "model/representation.h"
#include "../common/shared_data.h"

#include <QObject>
#include <QPixmap>

class ImageModel;
class IllumModel;
class FalseColorModel;
class UsSegmentationModel;
class LabelingModel;

class Controller;

class MainWindow;

class BandDock;
class LabelingDock;
class NormDock;
class ROIDock;
class RgbDock;
class IllumDock;
class GraphSegmentationDock;
class UsSegmentationDock;

#include <QObject>

class DockController : public QObject
{
	Q_OBJECT
public:
	explicit DockController(Controller *chief);
	void init();
signals:
	void rgbRequested();
public slots:
	void enableDocks(bool enable, TaskType tt);
protected slots:
	void raiseGraphSegDock(bool enable);
protected:
	/* Create dock widget objects. */
	void createDocks();

	/* Initialize signal/slot connections. */
	void setupDocks();

	Controller* chief;

	BandDock *bandDock;
	LabelingDock *labelingDock;
	NormDock *normDock;
	ROIDock *roiDock;
	RgbDock *rgbDock;
	IllumDock *illumDock;
	GraphSegmentationDock *graphSegDock;
	UsSegmentationDock *usSegDock;
};

#endif // DOCKCONTROLLER_H

