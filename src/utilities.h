/*
 *	Copyright (c) 2025-2026, Signaloid.
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include "common.h"


#define kDefaultInputDistributionConstantAnnualInterestRateMin  (0.5)
#define kDefaultInputDistributionConstantAnnualInterestRateMax  (1.0)
#define kDefaultInputDistributionConstantAnnualContributionMin  (5000.0)
#define kDefaultInputDistributionConstantAnnualContributionMax  (10000.0)
#define kDefaultInputDistributionConstantTaxRateInterestMin     (20.0)
#define kDefaultInputDistributionConstantTaxRateInterestMax     (40.0)
#define kDefaultInputDistributionConstantWithdrawalRateMin      (20.0)
#define kDefaultInputDistributionConstantWithdrawalRateMax      (40.0)

typedef enum
{
	kDemoFinanceIraDefaultNumberOfYearsToRetirement = 20,
} DemoFinanceIraDefault;

#ifdef NO_OS_AVAILABLE
typedef enum
{
	kDemoFinanceIraNoOSNumberOfYearsToRetirement = 2,
} DemoFinanceIraNoOS;
#endif

typedef enum
{
	kInputDistributionIndexTotalAnnualContributionToAccount = 0,
	kInputDistributionIndexCompoundedAnnualInterestRate     = 1,
	kInputDistributionIndexWithdrawalRate                   = 2,
	kInputDistributionIndexAssumedTaxRateOnInterest         = 3,
	kInputDistributionIndexMax                              = 4
} InputDistributionIndex;

typedef enum
{
	kOutputDistributionIndexFutureValueTaxed            = 0,
	kOutputDistributionIndexFutureValueTaxedWithdrawal  = 1,
	kOutputDistributionIndexMax                         = 2
} OutputDistributionIndex;

typedef struct
{
	CommonCommandLineArguments  common;

	int                         numberOfYearsToRetirement;
	char                        inputVariablesUxStrings[kInputDistributionIndexMax][kCommonConstantMaxCharsPerLine];
	bool                        isInputVariableSet[kInputDistributionIndexMax];
} CommandLineArguments;

/**
 *	@brief	Print out command-line usage.
 */
void
printUsage(void);

/**
 *	@brief	Get command-line arguments.
 *
 *		In the no-OS build there is no command line: `argc` and `argv` are
 *		ignored and the hard-coded configuration set by
 *		`setNoOSCommandLineArguments()` is used instead.
 *
 *	@param	argc		: Argument count from `main()`.
 *	@param	argv		: Argument vector from `main()`.
 *	@param	arguments	: Pointer to struct to store arguments.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
getCommandLineArguments(
	int                     argc,
	char *                  argv[],
	CommandLineArguments *  arguments);

/**
 *	@brief	Set the default values for the command-line arguments.
 *
 *	@param	arguments	: Command-line arguments pointer.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
setDefaultCommandLineArguments(CommandLineArguments * arguments);

#ifdef NO_OS_AVAILABLE

/**
 *	@brief	Set the hard-coded command-line arguments used by the no-OS build.
 *
 *		This is the single place to change the configuration that no-OS runs
 *		use, since those runs cannot be given command-line arguments. Fields
 *		not set here keep the values from `setDefaultCommandLineArguments()`.
 *
 *	@param	arguments	: Command-line arguments pointer.
 *	@return			: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
setNoOSCommandLineArguments(CommandLineArguments * arguments);
#endif

/**
 *	@brief	Set distributions for input variables via UxHw calls.
 *
 *	@param	arguments			: Pointer to command-line arguments struct.
 *	@param	inputVariables			: The input variables to be set.
 */
void
setInputVariables(
	CommandLineArguments *  arguments,
	double *                inputVariables[kInputDistributionIndexMax]);

/**
 *	@brief	Read the input variables from a CSV file.
 *
 *	@param	arguments		: Pointer to command-line arguments struct.
 *	@param	CSVInputVariables	: The input variables to be set.
 *	@return				: `kCommonConstantReturnTypeSuccess` if successful, else `kCommonConstantReturnTypeError`.
 */
CommonConstantReturnType
prepareCSVInputVariables(
	CommandLineArguments *  arguments,
	double *                CSVInputVariables[kInputDistributionIndexMax]);
