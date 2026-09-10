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

#include <stddef.h>
#include <stdbool.h>
#include "kernel.h"
#include "investment-retirement-account-uxhw.h"
#include "investment-retirement-account-monte-carlo.h"
#include "utilities.h"

double
calculateOutputUxHw(
	CommandLineArguments *  arguments,
	double *                inputVariables[kInputDistributionIndexMax],
	double *                outputVariables)
{
	double  result = 0.0;
	size_t  numberOfYearsToRetirement   = (size_t) arguments->numberOfYearsToRetirement;
	bool    calculateAllOutputs         = (arguments->common.outputSelect == kOutputDistributionIndexMax);

	/*
	 *	Set the per-year input distributions via UxHw calls, unless they have
	 *	already been read from a CSV file.
	 */
	if (!arguments->common.isInputFromFileEnabled)
	{
		setInputVariables(arguments, inputVariables);
	}

	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputDistributionIndexFutureValueTaxed))
	{
		result = outputVariables[kOutputDistributionIndexFutureValueTaxed] = futureValueTaxedUxHw(
			numberOfYearsToRetirement,
			inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount],
			inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate],
			inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest]
		);
	}

	if (calculateAllOutputs || (arguments->common.outputSelect == kOutputDistributionIndexFutureValueTaxedWithdrawal))
	{
		result = outputVariables[kOutputDistributionIndexFutureValueTaxedWithdrawal] = futureValueTaxFreeWithWithdrawalTaxUxHw(
			numberOfYearsToRetirement,
			inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount],
			inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate],
			inputVariables[kInputDistributionIndexWithdrawalRate]
		);
	}

	return result;
}

double
calculateOutputMonteCarlo(
	CommandLineArguments *  arguments,
	double *                inputVariables[kInputDistributionIndexMax],
	double *                outputVariables,
	double *                monteCarloOutputSamples)
{
	double  result = 0.0;
	size_t  numberOfYearsToRetirement = (size_t) arguments->numberOfYearsToRetirement;

	/*
	 *	`getCommandLineArguments()` rejects Monte Carlo mode unless a single
	 *	output is selected, so `outputSelect` is always one of the two output
	 *	indices here. Fall back to the first output rather than indexing
	 *	`outputVariables` out of bounds should that ever stop holding.
	 */
	size_t outputSelect = (arguments->common.outputSelect < kOutputDistributionIndexMax)
	                ? arguments->common.outputSelect
	                : kOutputDistributionIndexFutureValueTaxed;

	for (size_t ii = 0; ii < arguments->common.numberOfMonteCarloIterations; ii++)
	{
		/*
		 *	Draw a fresh set of per-year input samples for this iteration,
		 *	unless the inputs have already been read from a CSV file, in
		 *	which case every iteration reuses the same inputs.
		 */
		if (!arguments->common.isInputFromFileEnabled)
		{
			setInputVariables(arguments, inputVariables);
		}

		if (outputSelect == kOutputDistributionIndexFutureValueTaxed)
		{
			result = futureValueTaxedMonteCarlo(
				numberOfYearsToRetirement,
				inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount],
				inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate],
				inputVariables[kInputDistributionIndexAssumedTaxRateOnInterest]
			);
		}
		else
		{
			result = futureValueTaxFreeWithWithdrawalTaxMonteCarlo(
				numberOfYearsToRetirement,
				inputVariables[kInputDistributionIndexTotalAnnualContributionToAccount],
				inputVariables[kInputDistributionIndexCompoundedAnnualInterestRate],
				inputVariables[kInputDistributionIndexWithdrawalRate]
			);
		}

		monteCarloOutputSamples[ii] = result;
	}

	outputVariables[outputSelect] = result;

	return result;
}
