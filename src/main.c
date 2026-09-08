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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <uxhw.h>
#include "utilities.h"
#include "kernel.h"
#include "common.h"

#ifdef NO_OS_AVAILABLE

void
returnZeroNoOS(void);
#endif

int
main(int argc, char *  argv[])
{
	CommandLineArguments    arguments;
	double *                inputVariables[kInputDistributionIndexMax]      = { 0 };
	double                  outputVariables[kOutputDistributionIndexMax]    = { 0 };
	double *                monteCarloOutputSamples = NULL;
	clock_t                 start                   = 0;
	clock_t                 end                     = 0;
	double                  cpuTimeUsedInSeconds    = 0.0;
	size_t                  numberOfYearsToRetirement;
	const char *            applicationDescription = "Personal Finance: Tax-Free Retirement Account (Roth IRA) or Keogh Plan";
	const char *            outputVariableNames[kOutputDistributionIndexMax] = {
		[kOutputDistributionIndexFutureValueTaxed]              = "futureValueTaxed",
		[kOutputDistributionIndexFutureValueTaxedWithdrawal]    = "futureValueTaxFreeWithWithdrawalTax",
	};
	const char *            outputVariableDescriptions[kOutputDistributionIndexMax] = {
		[kOutputDistributionIndexFutureValueTaxed]              = "Future value, for yearly taxable payments (compounded annually)",
		[kOutputDistributionIndexFutureValueTaxedWithdrawal]    =
			"Future value, for yearly tax-free payments (compounded annually)",
	};
	/*
	 *	Both outputs are the future account value along a single simulated
	 *	path (UxHw path or Monte Carlo sample), not a summary statistic
	 *	derived from the whole sample set, so both are distributions.
	 */
	kOutputVariableTypeIndex outputVariableTypes[kOutputDistributionIndexMax] = {
		[kOutputDistributionIndexFutureValueTaxed]              = kOutputVariableTypeDistribution,
		[kOutputDistributionIndexFutureValueTaxedWithdrawal]    = kOutputVariableTypeDistribution,
	};

	/*
	 *	Get command-line arguments. In the no-OS build there is no command
	 *	line, so this instead applies the hard-coded configuration in
	 *	`setNoOSCommandLineArguments()`.
	 */
	if (getCommandLineArguments(argc, argv, &arguments) != kCommonConstantReturnTypeSuccess)
	{
		return kCommonConstantReturnTypeError;
	}

	numberOfYearsToRetirement = (size_t) arguments.numberOfYearsToRetirement;

	/*
	 *	Allocate one per-year input array per input variable. Guard against a
	 *	zero-year configuration, for which `checkedMalloc()` would otherwise
	 *	be asked for a zero-sized allocation.
	 */
	for (size_t ii = 0; ii < kInputDistributionIndexMax; ii++)
	{
		inputVariables[ii] = (double *) checkedMalloc(
			(numberOfYearsToRetirement > 0 ? numberOfYearsToRetirement : 1) * sizeof(double),
			__FILE__,
			__LINE__
		);
	}

	/*
	 *	Read input distributions from CSV if input from file is enabled. The
	 *	kernels then reuse these inputs instead of drawing their own.
	 */
	if (arguments.common.isInputFromFileEnabled)
	{
		if (prepareCSVInputVariables(&arguments, inputVariables) != kCommonConstantReturnTypeSuccess)
		{
			return kCommonConstantReturnTypeError;
		}
	}

	monteCarloOutputSamples =
		(double *) checkedMalloc(
			(arguments.common.numberOfMonteCarloIterations > 0
			        ? arguments.common.numberOfMonteCarloIterations
			        : 1) * sizeof(double),
			__FILE__,
			__LINE__
		);

	/*
	 *	Start timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		start = clock();
	}

	/*
	 *	`outputSelect` is only ever `kOutputDistributionIndexMax` (meaning
	 *	"all outputs") outside Monte Carlo mode, since `getCommandLineArguments()`
	 *	rejects that combination; so this indexes `outputVariableTypes` safely
	 *	whenever we are in Monte Carlo mode.
	 */
	bool isSelectedOutputScalar =
		(arguments.common.outputSelect != kOutputDistributionIndexMax)
		&& (outputVariableTypes[arguments.common.outputSelect] == kOutputVariableTypeScalar);

	/*
	 *	Dispatch to the mode-specific kernel. The Monte Carlo loop lives
	 *	inside `calculateOutputMonteCarlo()`; UxHw mode runs a single
	 *	distributional evaluation inside `calculateOutputUxHw()`.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		calculateOutputMonteCarlo(&arguments, inputVariables, outputVariables, monteCarloOutputSamples);

		if (!isSelectedOutputScalar)
		{
			MeanAndVariance meanAndVariance = calculateMeanAndVarianceOfDoubleSamples(
				monteCarloOutputSamples,
				arguments.common.numberOfMonteCarloIterations
			);

			outputVariables[arguments.common.outputSelect] = meanAndVariance.mean;
		}
	}
	else
	{
		calculateOutputUxHw(&arguments, inputVariables, outputVariables);
	}

	/*
	 *	Stop timing.
	 */
	if (arguments.common.isTimingEnabled)
	{
		end = clock();
		cpuTimeUsedInSeconds = ((double) (end - start)) / CLOCKS_PER_SEC;
	}

	CommonCommandLineArguments printArguments = arguments.common;

	if (arguments.common.isMonteCarloMode && isSelectedOutputScalar)
	{
		printArguments.isMonteCarloMode             = false;
		printArguments.numberOfMonteCarloIterations = 1;
	}

	/*
	 *	Print json outputs if in JSON output mode.
	 */
	if (arguments.common.isOutputJSONMode)
	{
		printJSONFormattedOutput(
			&printArguments,
			monteCarloOutputSamples,
			outputVariables,
			outputVariableDescriptions,
			kOutputDistributionIndexMax,
			applicationDescription
		);
	}
	/*
	 *	Print human-consumable output if not in JSON output mode.
	 */
	else
	{
		printHumanConsumableOutput(
			&printArguments,
			kOutputDistributionIndexMax,
			outputVariables,
			outputVariableNames,
			outputVariableDescriptions,
			monteCarloOutputSamples
		);
	}

	/*
	 *	Print timing result.
	 */
	if (arguments.common.isTimingEnabled)
	{
		printf("\nCPU time used: %" SignaloidParticleModifier "lf seconds\n", cpuTimeUsedInSeconds);
	}

	/*
	 *	Save Monte Carlo data to "data.out" if in Monte Carlo mode.
	 */
	if (arguments.common.isMonteCarloMode)
	{
		size_t samplesToSave = isSelectedOutputScalar
		    ? 1
		    : arguments.common.numberOfMonteCarloIterations;

		saveMonteCarloDoubleDataToDataDotOutFile(
			monteCarloOutputSamples,
			(uint64_t) (cpuTimeUsedInSeconds * 1000000),
			samplesToSave
		);
	}
	/*
	 *	Write output data if not in Monte Carlo mode and write to file is enabled.
	 */
	else if (arguments.common.isWriteToFileEnabled)
	{
		if (writeOutputDoubleDistributionsToCSV(
				arguments.common.outputFilePath,
				outputVariables,
				outputVariableNames,
				kOutputDistributionIndexMax
		))
		{
			fprintf(stderr, "Error: Could not write to output CSV file \"%s\".\n", arguments.common.outputFilePath);

			return kCommonConstantReturnTypeError;
		}
	}

	/*
	 *	Free dynamically-allocated memory.
	 */
	free(monteCarloOutputSamples);

	for (size_t ii = 0; ii < kInputDistributionIndexMax; ii++)
	{
		free(inputVariables[ii]);
	}

#ifdef NO_OS_AVAILABLE
	returnZeroNoOS();
#endif

	return kCommonConstantReturnTypeSuccess;
}
