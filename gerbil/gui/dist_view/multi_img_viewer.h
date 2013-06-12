/*
	Copyright(c) 2012 Johannes Jordan <johannes.jordan@cs.fau.de>.
	Copyright(c) 2012 Petr Koupy <petr.koupy@gmail.com>

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef MULTI_IMG_VIEWER_H
#define MULTI_IMG_VIEWER_H

#include "../model/representation.h"
#include "ui_multi_img_viewer.h"
#include "compute.h"
#include "viewport.h"
#include "viewportcontrol.h"

#include "../gerbil_gui_debug.h"

#include <multi_img.h>

#include <shared_data.h>
#include <background_task.h>
#include <background_task_queue.h>

#include <vector>
#include <map>
#include <QMenu>

class multi_img_viewer : public QWidget, private Ui::multi_img_viewer {
    Q_OBJECT
public:
	multi_img_viewer(QWidget *parent = 0);
	~multi_img_viewer();

	SharedMultiImgPtr getImage() { return image; }
	Viewport* getViewport() { return viewport; }
	void activateViewport() { viewport->activate(); }
	const cv::Mat1b& getHighlightMask() { return highlightMask; }
	int getSelection() { return viewport->selection; }
	void setSelection(int band) { viewport->selection = band; }
	representation::t getType() { return type; }
	void setType(representation::t type);
	bool isPayloadHidden() { return viewportGV->isHidden(); }

	BackgroundTaskQueue *queue;
	cv::Mat1s labels;

public slots:
	void updateMask(int dim);
	void subPixels(const std::map<std::pair<int, int>, short> &points);
	void addPixels(const std::map<std::pair<int, int>, short> &points);
	void subImage(sets_ptr temp, const std::vector<cv::Rect> &regions, cv::Rect roi);
	void addImage(sets_ptr temp, const std::vector<cv::Rect> &regions, cv::Rect roi);
	void setImage(SharedMultiImgPtr image, cv::Rect roi);
	void setIlluminant(cv::Mat1f illum);
	void setIlluminantIsApplied(bool applied=true);
	void showIlluminationCurve(bool show);
	void changeBinCount(int bins);
	void updateBinning(int bins);
	void finishBinCountChange(bool success);

	// controller stuff, actually
	void updateLabels(const cv::Mat1s& labels,
					  const QVector<QColor> &colors = QVector<QColor>(),
					  bool colorsChanged = false);
	void updateLabelsPartially(const cv::Mat1s &labels, const cv::Mat1b &mask);

	void toggleFold();
	void toggleLabeled(bool toggle);
	void toggleUnlabeled(bool toggle);
	void toggleLabels(bool toggle);
	void toggleLimiters(bool toggle);
	void overlay(int x, int y);
	void setActive()	{ viewport->active = true; viewport->update(); }
	void setInactive()	{  viewport->active = false; viewport->update(); }

signals:
	void newOverlay();
	void folding();
	void setGUIEnabled(bool enable, TaskType tt);
	void toggleViewer(bool enable, representation::t type);
	void finishTask(bool success);
	void finishedCalculation(representation::t type);

protected:
    void changeEvent(QEvent *e);

	// helpers for createMask
	void fillMaskSingle(int dim, int sel);
	void fillMaskLimiters(const std::vector<std::pair<int, int> >& limits);
	void updateMaskLimiters(const std::vector<std::pair<int, int> >&, int dim);
	void setTitle(representation::t type, multi_img::Value min, multi_img::Value max);

	Viewport *viewport;
	ViewportControl *control;
	SharedMultiImgPtr image;
	representation::t type;
	cv::Mat1f illuminant;
	bool ignoreLabels;
	cv::Mat1b highlightMask;

protected slots:
	void binningUpdate(bool updated);
	void binningRangeUpdate(bool updated);

};

#endif // MULTI_IMG_VIEWER_H