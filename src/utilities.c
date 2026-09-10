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

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <uxhw.h>
#include "utilities.h"

/**
 *	@brief	Return the default distributional value for the compounded annual interest rate.
 *
 *	@return	: The distributional value for the compounded annual interest rate.
 */
static double
getDefaultCompoundedAnnualInterestRate()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantAnnualInterestRateMin,
		kDefaultInputDistributionConstantAnnualInterestRateMax
	);
}

/**
 *	@brief	Return the default distributional value for the total annual contribution to account.
 *
 *	@return	: The distributional value for the total annual contribution to account.
 */
static double
getDefaultTotalAnnualContributionToAccount()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantAnnualContributionMin,
		kDefaultInputDistributionConstantAnnualContributionMax
	);
}

/**
 *	@brief	Return the default distributional value for the assumed tax rate on interest.
 *
 *	@return	: The distributional value for the assumed tax rate on interest.
 */
static double
getDefaultAssumedTaxRateOnInterest()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantTaxRateInterestMin,
		kDefaultInputDistributionConstantTaxRateInterestMax
	);
}

/**
 *	@brief	Return the default distributional value for withdrawal rate.
 *
 *	@return	: The distributional value for the withdrawal rate.
 */
static double
getDefaultWithdrawalRate()
{
	return UxHwDoubleUniformDist(
		kDefaultInputDistributionConstantWithdrawalRateMin,
		kDefaultInputDistributionConstantWithdrawalRateMax
	);
}

void
setInputVariables(
	CommandLineArguments *  arguments,
	double *                inputVariables[kInputDistributionIndexMax])
{
	for (int ii = 0; ii < arguments->numberOfYearsToRetirement; ii++)
	{
		/*
		 *	Check if the input variables are set using command-line arguments.
		 *	If no, set the default values using UxHw calls.
		 *	If yes, set the values by scanning the input strings obtained from the command-line.
		 *
		 *	Note: To get independent copies for a distribution specified from the command-line as
		 *	a Ux string, we used to use `UxHwDoubleCopyDistShape()`. This method worked as intended
		 *	for non-reference Laplace cores, however it did not work correctly for C0-Reference
		 *	(or for the native Monte Carlo (via `compat` submodule). So, instead, we now propagate
		 *	the input strings from the command-line (instead of the corresponding distribution) and
		 *	use `scanf()` at each iteration of this loop. This way C0-Reference also works correctly.
		 */
		if (arguments->isInputVariableSet[kInputDistributionIndexTotalAnnualContributionToAccount])
		{
			sscanf(
				arguments->inputVariablesUxStrings[kInputDistributionIndexTotalAnnualContributionToAccount], "%lf",
				&inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount][ii]
			);
		}
		else
		{
			inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount][ii] = getDefaultTotalAnnualContributionToAccount();
		}

		if (arguments->isInputVariableSet[kInputDistributionIndexCompoundedAnnualInterestRate])
		{
			sscanf(
				arguments->inputVariablesUxStrings[kInputDistributionIndexCompoundedAnnualInterestRate], "%lf",
				&inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate][ii]
			);
		}
		else
		{
			inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate][ii] = getDefaultCompoundedAnnualInterestRate();
		}

		if (arguments->isInputVariableSet[kInputDistributionIndexAssumedTaxRateOnInterest])
		{
			sscanf(
				arguments->inputVariablesUxStrings[kInputDistributionIndexAssumedTaxRateOnInterest], "%lf",
				&inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest][ii]
			);
		}
		else
		{
			inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest][ii] = getDefaultAssumedTaxRateOnInterest();
		}

		if (arguments->isInputVariableSet[kInputDistributionIndexWithdrawalRate])
		{
			sscanf(
				arguments->inputVariablesUxStrings[kInputDistributionIndexWithdrawalRate], "%lf",
				&inputVariables[kInputDistributionIndexWithdrawalRate][ii]
			);
		}
		else
		{
			inputVariables[kInputDistributionIndexWithdrawalRate][ii] = getDefaultWithdrawalRate();
		}
	}

	return;
}

CommonConstantReturnType
setDefaultCommandLineArguments(CommandLineArguments * arguments)
{
	if (arguments == NULL)
	{
		fprintf(stderr, "Error: Arguments pointer is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

/*
 *	Older GCC versions have a bug which gives a spurious warning for the C universal zero
 *	initializer `{0}`. Any workaround makes the code less portable or prevents the common code
 *	from adding new fields to the `CommonCommandLineArguments` struct. Therefore, we surpress
 *	this warning.
 *
 *	See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"

	/*
	 *	This zero-fills every Ux string and clears every `isInputVariableSet`
	 *	flag, so an unset input variable falls back to its default
	 *	distribution in `setInputVariables()`.
	 */
	*arguments = (CommandLineArguments)
	{
		.common = (CommonCommandLineArguments) { 0 },
		.numberOfYearsToRetirement  = kDemoFinanceIraDefaultNumberOfYearsToRetirement,
		.inputVariablesUxStrings    = { "" },
		.isInputVariableSet         = { false },
	};

#pragma GCC diagnostic pop

	return kCommonConstantReturnTypeSuccess;
}

#ifdef NO_OS_AVAILABLE
CommonConstantReturnType
setNoOSCommandLineArguments(CommandLineArguments * arguments)
{
	/*
	 *	Start from the defaults so that every demo-specific field is
	 *	initialized, then override the ones the no-OS build fixes.
	 */
	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	arguments->common.isWriteToFileEnabled          = false;
	arguments->common.isTimingEnabled               = false;
	arguments->common.numberOfMonteCarloIterations  = 1;
	arguments->common.outputSelect                  = kOutputDistributionIndexMax;
	arguments->common.isOutputSelected              = true;
	arguments->common.isVerbose                 = false;
	arguments->common.isInputFromFileEnabled    = false;
	arguments->common.isOutputJSONMode          = false;
	arguments->common.isHelpEnabled             = false;
	arguments->common.isMonteCarloMode          = false;
	arguments->common.isSingleShotExecution     = true;

	arguments->numberOfYearsToRetirement = kDemoFinanceIraNoOSNumberOfYearsToRetirement;

 #ifdef NUMBER_OF_YEARS_TO_RETIREMENT
	arguments->numberOfYearsToRetirement = (int) NUMBER_OF_YEARS_TO_RETIREMENT;
 #endif
 #ifdef COMPOUNDED_ANNUAL_INTEREST_RATE
	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexCompoundedAnnualInterestRate], kCommonConstantMaxCharsPerLine, "%s",
		(char *) COMPOUNDED_ANNUAL_INTEREST_RATE
	);
	arguments->isInputVariableSet[kInputDistributionIndexCompoundedAnnualInterestRate] = true;
 #endif
 #ifdef TOTAL_ANNUAL_CONTRIBUTION_TO_ACCOUNT
	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexTotalAnnualContributionToAccount], kCommonConstantMaxCharsPerLine, "%s",
		(char *) TOTAL_ANNUAL_CONTRIBUTION_TO_ACCOUNT
	);
	arguments->isInputVariableSet[kInputDistributionIndexTotalAnnualContributionToAccount] = true;
 #endif
 #ifdef ASSUMED_TAX_RATE_ON_INTEREST_ARG
	snprintf(
		arguments->inputVariablesUxStrings[kInputDistributionIndexAssumedTaxRateOnInterest], kCommonConstantMaxCharsPerLine, "%s",
		(char *) ASSUMED_TAX_RATE_ON_INTEREST_ARG
	);
	arguments->isInputVariableSet[kInputDistributionIndexAssumedTaxRateOnInterest] = true;
 #endif
 #ifdef WITHDRAWAL_RATE
	snprintf(arguments->inputVariablesUxStrings[kInputDistributionIndexWithdrawalRate], kCommonConstantMaxCharsPerLine, "%s", (char *) WITHDRAWAL_RATE);
	arguments->isInputVariableSet[kInputDistributionIndexWithdrawalRate] = true;
 #endif

	return kCommonConstantReturnTypeSuccess;
}
#endif

void
printUsage(void)
{
	fprintf(
		stderr,
		"Example: Personal Finance: Tax-Free Retirement Account (Roth IRA) or Keogh Plan - "
		"Signaloid version\n"
	);
	fprintf(stderr, "\n");

	fprintf(stderr, "Usage: Valid command-line arguments are:\n");
	fprintf(
		stderr,
		"\t[-i, --input <Path to input CSV file : str>] (Read inputs from file.)\n"
		"\t[-o, --output <Path to output CSV file : str>] (Specify the output file.)\n"
		"\t[-S, --select-output <output : int>] (Compute 0-indexed output. By default, all outputs are computed.)\n"
		"\t[-M, --multiple-executions <Number of executions : int> (Default: 1)] (Repeated execute kernel for benchmarking.)\n"
		"\t[-T, --time] (Timing mode: Times and prints the timing of the kernel execution.)\n"
		"\t[-b, --benchmarking] (Benchmarking mode: Generate outputs in format for benchmarking.)\n"
		"\t[-j, --json] (Print output in JSON format.)\n"
		"\t[-h, --help] (Display this help message.)\n"
		"\t[-n, --number-of-years <The number of years to retirement : int in [0, inf)> (Default: %d)]\n"
		"\t[-c, --compounded-annual-interest-rate <The compounded annual interest rate expressed as a percentage: double> (Default: Uniform(%"SignaloidParticleModifier
		".1f, %"SignaloidParticleModifier".1f))]\n"
		"\t[-t, --total-annual-contribution-to-account <The total annual contribution to the account : double> (Default: Uniform(%"SignaloidParticleModifier
		".1f, %"SignaloidParticleModifier".1f))]\n"
		"\t[-r, --assumed-tax-rate-on-interest <The assumed tax rate on interest expressed as a percentage : double> (Default: Uniform(%"SignaloidParticleModifier
		".1f, %"SignaloidParticleModifier".1f))]\n"
		"\t[-w, --withdrawal-rate <The withdrawal rate expressed as a percentage : double> (Default: Uniform(%"SignaloidParticleModifier
		".1f, %"SignaloidParticleModifier".1f))]\n",
		kDemoFinanceIraDefaultNumberOfYearsToRetirement,
		kDefaultInputDistributionConstantAnnualInterestRateMin,
		kDefaultInputDistributionConstantAnnualInterestRateMax,
		kDefaultInputDistributionConstantAnnualContributionMin,
		kDefaultInputDistributionConstantAnnualContributionMax,
		kDefaultInputDistributionConstantTaxRateInterestMin,
		kDefaultInputDistributionConstantTaxRateInterestMax,
		kDefaultInputDistributionConstantWithdrawalRateMin,
		kDefaultInputDistributionConstantWithdrawalRateMax
	);

	fprintf(stderr, "\n");

	return;
}

CommonConstantReturnType
getCommandLineArguments(int argc, char *  argv[], CommandLineArguments *  arguments)
{
#ifdef NO_OS_AVAILABLE
	/*
	 *	The no-OS build has no command line to parse, so ignore `argc` and
	 *	`argv` and use the hard-coded configuration instead.
	 */
	(void) argc;
	(void) argv;

	puts("Using hard coded command line arguments");

	return setNoOSCommandLineArguments(arguments);

#else
	const char *    numberOfYearsToRetirementArg        = NULL;
	const char *    compoundedAnnualInterestRateArg     = NULL;
	const char *    totalAnnualContributionToAccountArg = NULL;
	const char *    assumedTaxRateOnInterestArg         = NULL;
	const char *    withdrawalRateArg           = NULL;
	bool            distributionalArgumentGiven = false;
	bool            inputVariableArgumentGiven  = false;
	const char      kConstantStringUx[]         = "Ux";

	if (arguments == NULL)
	{
		fprintf(stderr, "Error: The provided pointer to arguments is NULL.\n");

		return kCommonConstantReturnTypeError;
	}

	if (setDefaultCommandLineArguments(arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	DemoOption options[] = {
		{
			.opt    = "n", .optAlternative = "number-of-years",
			.hasArg = true, .foundArg = &numberOfYearsToRetirementArg, .foundOpt = NULL
		},
		{
			.opt    = "c", .optAlternative = "compounded-annual-interest-rate",
			.hasArg = true, .foundArg = &compoundedAnnualInterestRateArg, .foundOpt = NULL
		},
		{
			.opt    = "t", .optAlternative = "total-annual-contribution-to-account",
			.hasArg = true, .foundArg = &totalAnnualContributionToAccountArg, .foundOpt = NULL
		},
		{
			.opt    = "r", .optAlternative = "assumed-tax-rate-on-interest",
			.hasArg = true, .foundArg = &assumedTaxRateOnInterestArg, .foundOpt = NULL
		},
		{
			.opt    = "w", .optAlternative = "withdrawal-rate",
			.hasArg = true, .foundArg = &withdrawalRateArg, .foundOpt = NULL
		},
		{ 0 },
	};

	if (parseArgs(argc, argv, &arguments->common, options) != kCommonConstantReturnTypeSuccess)
	{
		fprintf(stderr, "Parsing command-line arguments failed\n");
		printUsage();

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isHelpEnabled)
	{
		printUsage();

		exit(EXIT_SUCCESS);
	}

	if (((strcmp(arguments->common.outputFilePath, "") != 0) || arguments->common.isWriteToFileEnabled) && (!arguments->common.isOutputJSONMode))
	{
		printf("The output will be stored in the file: %s\n", arguments->common.outputFilePath);
	}

	if (!arguments->common.isOutputSelected)
	{
		arguments->common.outputSelect = kOutputDistributionIndexMax;
	}
	else if (arguments->common.outputSelect > kOutputDistributionIndexMax)
	{
		fprintf(stderr, "Error: Output select option must be in the range [0, %d].\n", kOutputDistributionIndexMax);

		return kCommonConstantReturnTypeError;
	}

	if (arguments->common.isVerbose)
	{
		fprintf(stderr, "Error: Verbose mode is not supported by this application.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	When all outputs are selected, we cannot be in Monte Carlo mode.
	 */
	if (arguments->common.outputSelect == kOutputDistributionIndexMax)
	{
		if (arguments->common.isMonteCarloMode)
		{
			fprintf(stderr, "Error: Please select a single output when in Monte Carlo mode.\n");

			return kCommonConstantReturnTypeError;
		}
	}

	if (numberOfYearsToRetirementArg != NULL)
	{
		int value;
		int ret = parseIntChecked(numberOfYearsToRetirementArg, &value);

		if (ret != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: The number of years to retirement must be an integer.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		if (value < 0)
		{
			fprintf(stderr, "Error: The number of years to retirement must be non-negative.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->numberOfYearsToRetirement = value;
	}

	if (compoundedAnnualInterestRateArg != NULL)
	{
		int ret = snprintf(
			arguments->inputVariablesUxStrings[kInputDistributionIndexCompoundedAnnualInterestRate], kCommonConstantMaxCharsPerLine, "%s",
			compoundedAnnualInterestRateArg
		);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(compoundedAnnualInterestRateArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the compounded annual interest rate from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexCompoundedAnnualInterestRate] = true;
	}

	if (totalAnnualContributionToAccountArg != NULL)
	{
		int ret = snprintf(
			arguments->inputVariablesUxStrings[kInputDistributionIndexTotalAnnualContributionToAccount], kCommonConstantMaxCharsPerLine, "%s",
			totalAnnualContributionToAccountArg
		);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(totalAnnualContributionToAccountArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the total annual contribution to account from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexTotalAnnualContributionToAccount] = true;
	}

	if (assumedTaxRateOnInterestArg != NULL)
	{
		int ret = snprintf(
			arguments->inputVariablesUxStrings[kInputDistributionIndexAssumedTaxRateOnInterest], kCommonConstantMaxCharsPerLine, "%s",
			assumedTaxRateOnInterestArg
		);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(assumedTaxRateOnInterestArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the assumed tax rate on interest from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexAssumedTaxRateOnInterest] = true;
	}

	if (withdrawalRateArg != NULL)
	{
		int ret = snprintf(arguments->inputVariablesUxStrings[kInputDistributionIndexWithdrawalRate], kCommonConstantMaxCharsPerLine, "%s", withdrawalRateArg);

		if (arguments->common.isMonteCarloMode)
		{
			if (strstr(withdrawalRateArg, kConstantStringUx) != NULL)
			{
				fprintf(stderr, "Error: Native Monte Carlo is not compatible with Ux strings from command line.\n");

				return kCommonConstantReturnTypeError;
			}
		}

		if ((ret < 0) || (ret >= kCommonConstantMaxCharsPerLine))
		{
			fprintf(stderr, "Error: Could not read the value of the withdrawal rate from command-line arguments.\n");
			printUsage();

			return kCommonConstantReturnTypeError;
		}

		arguments->isInputVariableSet[kInputDistributionIndexWithdrawalRate] = true;
	}

	/*
	 *	Record whether any input variable was given on the command line, and
	 *	whether any of those was a distributional (`Ux`) value, so that the
	 *	checks below can act on them.
	 */
	const char *const inputVariableArgs[] = {
		compoundedAnnualInterestRateArg,
		totalAnnualContributionToAccountArg,
		assumedTaxRateOnInterestArg,
		withdrawalRateArg,
	};

	for (size_t ii = 0; ii < sizeof(inputVariableArgs) / sizeof(inputVariableArgs[0]); ii++)
	{
		if (inputVariableArgs[ii] == NULL)
		{
			continue;
		}

		inputVariableArgumentGiven = true;

		if (strstr(inputVariableArgs[ii], kConstantStringUx) != NULL)
		{
			distributionalArgumentGiven = true;
		}
	}

	/*
	 *	Monte Carlo mode does not work with command-line parameters.
	 */
	if (distributionalArgumentGiven && (arguments->common.isMonteCarloMode))
	{
		fprintf(stderr, "Error: Monte Carlo mode cannot work with distributional arguments.\n");

		return kCommonConstantReturnTypeError;
	}

	/*
	 *	We prioritize inputs given from CSV file.
	 */
	if (inputVariableArgumentGiven && (arguments->common.isInputFromFileEnabled))
	{
		fprintf(stderr, "Warning: When reading data from an input file, input variable arguments are ignored.\n");
	}

	return kCommonConstantReturnTypeSuccess;

#endif
}

CommonConstantReturnType
prepareCSVInputVariables(
	CommandLineArguments *  arguments,
	double *                CSVInputVariables[kInputDistributionIndexMax])
{
	const char * inputCSVHeaders[] = {
		"total_annual_contribution",
		"compounded_annual_interest_percentage",
		"withdrawal_rate_percentage",
		"assumed_tax_rate_on_interest_percentage"
	};

	size_t numberOfYearsToRetirement = (size_t) arguments->numberOfYearsToRetirement;

	/*
	 *	We deliberately re-read the CSV file once per year rather than reading
	 *	it once and reusing the result. Each call builds its distributions with
	 *	a fresh `UxHwDoubleDistFromSamples()`, so every year gets an independent
	 *	copy of each input distribution. Hoisting the read out of this loop
	 *	would instead share one variable across all years, which correlation-
	 *	tracking cores would (correctly) treat as perfectly correlated years and
	 *	which would not match the per-year independent draws that
	 *	`setInputVariables()` performs for the default inputs.
	 */
	for (size_t ii = 0; ii < numberOfYearsToRetirement; ii++)
	{
		double                      tempInputVariables[kInputDistributionIndexMax];
		CommonConstantReturnType    ret;

		ret = readInputDoubleDistributionsFromCSV(
			arguments->common.inputFilePath,
			inputCSVHeaders,
			tempInputVariables,
			kInputDistributionIndexMax
		);

		if (ret != kCommonConstantReturnTypeSuccess)
		{
			fprintf(stderr, "Error: Could not read from input CSV file \"%s\".\n", arguments->common.inputFilePath);

			return kCommonConstantReturnTypeError;
		}

		for (size_t jj = 0; jj < kInputDistributionIndexMax; jj++)
		{
			CSVInputVariables[jj][ii] = tempInputVariables[jj];
		}
	}

	return kCommonConstantReturnTypeSuccess;
}
