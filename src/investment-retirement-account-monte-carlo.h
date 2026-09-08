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

/**
 *	@brief	Calculate one Monte Carlo sample of the future value of a retirement
 *		account funded with yearly taxable payments, compounded annually.
 *		Each element of the per-year input arrays is a single sample drawn
 *		from the corresponding input distribution, so the returned value is
 *		one sample of the future-value distribution. Unlike its UxHw
 *		counterpart in `investment-retirement-account-uxhw.h`, this
 *		implementation uses no UxHw distributional API and therefore also
 *		builds against the native (non-Signaloid) Monte Carlo toolchain.
 *
 *	@param	numberOfYearsToRetirement		: Number of years of compounding to apply.
 *	@param	totalAnnualContributionToAccount	: Per-year sample of the total contribution to the account.
 *	@param	compoundedAnnualInterestRate		: Per-year sample of the compounded annual interest rate, expressed as a percentage.
 *	@param	assumedTaxRateOnInterest		: Per-year sample of the assumed tax rate on the interest, expressed as a percentage.
 *	@return	double					: Returns one sample of the future value of the account.
 */
double
futureValueTaxedMonteCarlo(
	size_t          numberOfYearsToRetirement,
	const double *  totalAnnualContributionToAccount,
	const double *  compoundedAnnualInterestRate,
	const double *  assumedTaxRateOnInterest);

/**
 *	@brief	Calculate one Monte Carlo sample of the future value of a tax-free
 *		retirement account, compounded annually. Each year's contribution is
 *		reduced by that year's withdrawal rate before it enters the account,
 *		and the interest it earns is not taxed. As above, the inputs are
 *		single samples of the input distributions and the result is one
 *		sample of the future-value distribution.
 *
 *	@param	numberOfYearsToRetirement		: Number of years of compounding to apply.
 *	@param	totalAnnualContributionToAccount	: Per-year sample of the total contribution to the account.
 *	@param	compoundedAnnualInterestRate		: Per-year sample of the compounded annual interest rate, expressed as a percentage.
 *	@param	withdrawalRate				: Per-year sample of the withdrawal rate, expressed as a percentage.
 *	@return	double					: Returns one sample of the future value of the account.
 */
double
futureValueTaxFreeWithWithdrawalTaxMonteCarlo(
	size_t          numberOfYearsToRetirement,
	const double *  totalAnnualContributionToAccount,
	const double *  compoundedAnnualInterestRate,
	const double *  withdrawalRate);
