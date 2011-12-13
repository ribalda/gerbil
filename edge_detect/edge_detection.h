//	-------------------------------------------------------------------------------------------------------------	 	//
// 														Variants of Self-Organizing Maps																											//
// Studienarbeit in Computer Vision at the Chair of Patter Recognition Friedrich-Alexander Universitaet Erlangen		//
// Start:	15.11.2010																																																//
// End	:	16.05.2011																																																//
// 																																																									//
// Ralph Muessig																																																		//
// ralph.muessig@e-technik.stud.uni-erlangen.de																																			//
// Informations- und Kommunikationstechnik																																					//
//	---------------------------------------------------------------------------------------------------------------	//


#ifndef EDGE_DETECTION_H
#define EDGE_DETECTION_H

#include "self_organizing_map.h"
#include "edge_detection_config.h"
#include <command.h>


class EdgeDetection : public vole::Command {

	public:
		EdgeDetection();
		// use only with pre-filled config!
		EdgeDetection(const vole::EdgeDetectionConfig &config);
		int execute();

		SOM *train(const multi_img& img);
		void printShortHelp() const;
		void printHelp() const;

	protected:
		int executeSimple();

		vole::EdgeDetectionConfig config;
		
};


#endif
