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
#include "investment-retirement-account-uxhw.h"

double
futureValueTaxedUxHw(
	size_t          numberOfYearsToRetirement,
	const double *  totalAnnualContributionToAccount,
	const double *  compoundedAnnualInterestRate,
	const double *  assumedTaxRateOnInterest)
{
	double  futureValue = 0.0;
	double  currentYearValueIncreaseRate;

	for (size_t ii = 0; ii < numberOfYearsToRetirement; ii++)
	{
		/*
		 *	The interest earned in the year is taxed, so only the
		 *	after-tax fraction of it increases the value of the account.
		 */
		currentYearValueIncreaseRate =
			(compoundedAnnualInterestRate[ii] / 100) *
			(1.0 - (assumedTaxRateOnInterest[ii] / 100));

		futureValue =
			(futureValue + totalAnnualContributionToAccount[ii]) *
			(1.0 + currentYearValueIncreaseRate);
	}

	return futureValue;
}

double
futureValueTaxFreeWithWithdrawalTaxUxHw(
	size_t          numberOfYearsToRetirement,
	const double *  totalAnnualContributionToAccount,
	const double *  compoundedAnnualInterestRate,
	const double *  withdrawalRate)
{
	double  futureValue = 0.0;
	double  currentYearValueIncreaseRate;

	for (size_t ii = 0; ii < numberOfYearsToRetirement; ii++)
	{
		/*
		 *	Contributions come out of income that has already been
		 *	taxed, so only the after-tax fraction of the year's
		 *	contribution enters the account. The interest it then
		 *	earns is not taxed.
		 */
		currentYearValueIncreaseRate = (compoundedAnnualInterestRate[ii] / 100);

		futureValue =
			(futureValue + totalAnnualContributionToAccount[ii] * (1.0 - (withdrawalRate[ii] / 100))) *
			(1.0 + currentYearValueIncreaseRate);
	}

	return futureValue;
}
