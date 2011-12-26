//system includes
#include <iostream>
#include <sstream>
#include <fstream>

//cv includes
#include <cv.h>
#include <highgui.h>

//vole includes
#include <stopwatch.h>

//som includes
#include "edge_detection.h"
#include "self_organizing_map.h"
#include "som_trainer.h"
#include "somtester.h"

EdgeDetection::EdgeDetection() 
  : vole::Command("edge_detect", // command name
          config,
		  "Johannes Jordan, Ralph Muessig", // author names
		  "johannes.jordan@cs.fau.de" ) // email
{}

EdgeDetection::EdgeDetection(const vole::EdgeDetectionConfig &cfg)
  : config(cfg), vole::Command("edge_detect", // command name
		  config,
		  "Johannes Jordan, Ralph Muessig", // author names
		  "johannes.jordan@cs.fau.de" ) // email
{}

SOM* EdgeDetection::train(const multi_img &img)
{
	SOM *som = new SOM(config, img.size());
	std::cout << "# Generated SOM " << config.width << "x" << config.height << " with dimension " << img.size() << std::endl;

	SOMTrainer trainer(*som, img, config);

	std::cout << "# SOM Trainer starts to feed the network using "<< config.maxIter << " iterations..." << std::endl;

	vole::Stopwatch watch("Training");
	trainer.feedNetwork();

	return som;
}

int EdgeDetection::execute()
{
	assert(!config.prefix_enabled); // input, output file variables set

	multi_img img;
	img.minval = 0.;
	img.maxval = 1.;
	img.read_image(config.input_file);
	if (img.empty())
		return -1;
	img.rebuildPixels(false);

	if (config.hack3d) {
		assert(config.height == 1);
		config.height = config.width * config.width;
	}

	SOM *som = train(img);
	if (config.output_som) {
		multi_img somimg = som->export_2d();
		somimg.write_out(config.output_dir + "/som");
		config.storeConfig((config.output_dir + "/config.txt").c_str());
	}

	std::cout << "# Generating 2D image using the SOM and the multispectral image..." << std::endl;
	vole::Stopwatch watch("Edge Image Generation");
	SOMTester tester(*som, img, config);

	cv::Mat1d dX, dY;
	tester.getEdge(dX, dY);
	cv::Mat sobelXShow, sobelYShow;

	dX.convertTo(sobelXShow, CV_8UC1, 255.);
	cv::imwrite(config.output_dir + "/dx.png", sobelXShow);

	dY.convertTo(sobelYShow, CV_8UC1, 255.);
	cv::imwrite(config.output_dir + "/dy.png", sobelYShow);
    
	delete som;
	return 0;
}


void EdgeDetection::printShortHelp() const 
{
	std::cout << "Edge detection in multispectral images using SOM." << std::endl;
}

void EdgeDetection::printHelp() const 
{
	std::cout << "Edge detection as described in Jordan, J., Elli A., Edge Detection in\n"
				 "Multispectral Images Using the N-Dimensional Self-Organizing Map. ICIP 2011."
				 << std::endl;
}