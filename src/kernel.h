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
#include "utilities.h"

/**
 *	@brief	UxHw-mode calculation kernel. Draws the per-year input
 *		distributions (unless they were read from a CSV file) and evaluates
 *		the selected output(s) once, using distributional arithmetic, via
 *		the kernels in `investment-retirement-account-uxhw.h`. Writes the
 *		per-output results into `outputVariables`.
 *
 *	@param	arguments		: Command-line arguments.
 *	@param	inputVariables		: Array of `kInputDistributionIndexMax` per-year input arrays, each of length `arguments->numberOfYearsToRetirement`.
 *	@param	outputVariables		: Array of size `kOutputDistributionIndexMax` to fill.
 *	@return	double			: Returns the value of the selected output (or of the last output when all outputs are selected).
 */
double
calculateOutputUxHw(
	CommandLineArguments *  arguments,
	double *                inputVariables[kInputDistributionIndexMax],
	double *                outputVariables);

/**
 *	@brief	Monte Carlo calculation kernel. Runs
 *		`arguments->common.numberOfMonteCarloIterations` independent
 *		iterations, redrawing a fresh set of per-year input samples for each
 *		one (unless they were read from a CSV file) and evaluating the
 *		selected output via the sample-domain kernels in
 *		`investment-retirement-account-monte-carlo.h`, which use no UxHw
 *		distributional API. Fills `monteCarloOutputSamples` with one sample
 *		per iteration and writes the last sample into
 *		`outputVariables[arguments->common.outputSelect]`.
 *
 *		A single output must be selected in Monte Carlo mode; this is
 *		enforced by `getCommandLineArguments()`.
 *
 *	@param	arguments		: Command-line arguments.
 *	@param	inputVariables		: Array of `kInputDistributionIndexMax` per-year input arrays, each of length `arguments->numberOfYearsToRetirement`.
 *	@param	outputVariables		: Array of size `kOutputDistributionIndexMax` to fill.
 *	@param	monteCarloOutputSamples	: Array of `numberOfMonteCarloIterations` doubles, filled with samples.
 *	@return	double			: Returns the last sample of the selected output.
 */
double
calculateOutputMonteCarlo(
	CommandLineArguments *  arguments,
	double *                inputVariables[kInputDistributionIndexMax],
	double *                outputVariables,
	double *                monteCarloOutputSamples);
