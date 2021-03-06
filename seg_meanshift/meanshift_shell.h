#ifndef MEANSHIFT_SHELL_H
#define MEANSHIFT_SHELL_H

#include "meanshift_config.h"
#include <command.h>

namespace seg_meanshift {

class MeanShiftShell : public shell::Command {
public:
	MeanShiftShell(ProgressObserver *po = NULL);
	~MeanShiftShell();
	int execute();
	std::map<std::string, boost::any>
	execute(std::map<std::string, boost::any> &input,
			ProgressObserver *po);

	void printShortHelp() const;
	void printHelp() const;

	MeanShiftConfig config;
};

}

#endif
