/*!
 * \file volk_gnsssdr_32fc_convert_8ic.h
 * \brief Volk protokernel: converts float32 complex values to 8 integer complex values taking care of overflow
 * \authors <ul>
 *          <li> Andres Cecilia, 2014. a.cecilia.luque(at)gmail.com
 *          </ul>
 *
 * -------------------------------------------------------------------------
 *
 * Copyright (C) 2010-2015  (see AUTHORS file for a list of contributors)
 *
 * GNSS-SDR is a software defined Global Navigation
 *          Satellite Systems receiver
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 *
 * -------------------------------------------------------------------------
 */


#include <inttypes.h>
#include <math.h>
#include <limits.h>
#include "volk_gnsssdr/volk_gnsssdr_complex.h"


#ifndef INCLUDED_volk_gnsssdr_32fc_convert_8ic_H
#define INCLUDED_volk_gnsssdr_32fc_convert_8ic_H

#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
 \brief Converts a float vector of 64 bits (32 bits each part) into a 16 integer vector (8 bits each part)
 \param inputVector The floating point input data buffer
 \param outputVector The 16 bit output data buffer
 \param num_points The number of data values to be converted
 */
static inline void volk_gnsssdr_32fc_convert_8ic_u_sse2(lv_8sc_t* outputVector, const lv_32fc_t* inputVector, unsigned int num_points)
{
    unsigned i = 0;
    const unsigned int sse_iters = num_points * 2 / 16;

    float* inputVectorPtr = (float*)inputVector;
    int8_t* outputVectorPtr = (int8_t*)outputVector;

    float min_val = SCHAR_MIN;
    float max_val = SCHAR_MAX;

    __m128 inputVal1, inputVal2, inputVal3, inputVal4;
    __m128i intInputVal1, intInputVal2, intInputVal3, intInputVal4;
    __m128i int8InputVal;
    __m128 ret1, ret2, ret3, ret4;
    __m128 vmin_val = _mm_set_ps1(min_val);
    __m128 vmax_val = _mm_set_ps1(max_val);

    for(;i < sse_iters; i++)
        {
            inputVal1 = _mm_loadu_ps((float*)inputVectorPtr); inputVectorPtr += 4;
            inputVal2 = _mm_loadu_ps((float*)inputVectorPtr); inputVectorPtr += 4;
            inputVal3 = _mm_loadu_ps((float*)inputVectorPtr); inputVectorPtr += 4;
            inputVal4 = _mm_loadu_ps((float*)inputVectorPtr); inputVectorPtr += 4;

            // Clip
            ret1 = _mm_max_ps(_mm_min_ps(inputVal1, vmax_val), vmin_val);
            ret2 = _mm_max_ps(_mm_min_ps(inputVal2, vmax_val), vmin_val);
            ret3 = _mm_max_ps(_mm_min_ps(inputVal3, vmax_val), vmin_val);
            ret4 = _mm_max_ps(_mm_min_ps(inputVal4, vmax_val), vmin_val);

            intInputVal1 = _mm_cvtps_epi32(ret1);
            intInputVal2 = _mm_cvtps_epi32(ret2);
            intInputVal3 = _mm_cvtps_epi32(ret3);
            intInputVal4 = _mm_cvtps_epi32(ret4);

            intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);
            intInputVal2 = _mm_packs_epi32(intInputVal3, intInputVal4);
            int8InputVal = _mm_packs_epi16(intInputVal1, intInputVal2);

            _mm_storeu_si128((__m128i*)outputVectorPtr, int8InputVal);
            outputVectorPtr += 16;
        }

    for(i = sse_iters * 16; i < num_points * 2; i++)
        {
            if(inputVectorPtr[i] > max_val)
                inputVectorPtr[i] = max_val;
            else if(inputVectorPtr[i] < min_val)
                inputVectorPtr[i] = min_val;
            *outputVectorPtr++ = (int8_t)rintf(*inputVectorPtr++);
        }
}
#endif /* LV_HAVE_SSE2 */

#ifdef LV_HAVE_GENERIC
/*!
 \brief Converts a float vector of 64 bits (32 bits each part) into a 16 integer vector (8 bits each part)
 \param inputVector The floating point input data buffer
 \param outputVector The 16 bit output data buffer
 \param num_points The number of data values to be converted
 */
static inline void volk_gnsssdr_32fc_convert_8ic_generic(lv_8sc_t* outputVector, const lv_32fc_t* inputVector, unsigned int num_points)
{
    float* inputVectorPtr = (float*)inputVector;
    int8_t* outputVectorPtr = (int8_t*)outputVector;
    float min_val = SCHAR_MIN;
    float max_val = SCHAR_MAX;

    for(unsigned int i = 0; i < num_points * 2; i++)
        {
            if(inputVectorPtr[i] > max_val)
                inputVectorPtr[i] = max_val;
            else if(inputVectorPtr[i] < min_val)
                inputVectorPtr[i] = min_val;
            outputVectorPtr[i] = (int8_t)rintf(inputVectorPtr[i]);
        }
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_SSE2
#include <emmintrin.h>
/*!
 \brief Converts a float vector of 64 bits (32 bits each part) into a 16 integer vector (8 bits each part)
 \param inputVector The floating point input data buffer
 \param outputVector The 16 bit output data buffer
 \param num_points The number of data values to be converted
 */
static inline void volk_gnsssdr_32fc_convert_8ic_a_sse2(lv_8sc_t* outputVector, const lv_32fc_t* inputVector, unsigned int num_points)
{
    const unsigned int sse_iters = num_points / 8;

    float* inputVectorPtr = (float*)inputVector;
    int8_t* outputVectorPtr = (int8_t*)outputVector;

    float min_val = SCHAR_MIN;
    float max_val = SCHAR_MAX;

    __m128 inputVal1, inputVal2, inputVal3, inputVal4;
    __m128i intInputVal1, intInputVal2, intInputVal3, intInputVal4;
    __m128i int8InputVal;
    __m128 ret1, ret2, ret3, ret4;
    __m128 vmin_val = _mm_set_ps1(min_val);
    __m128 vmax_val = _mm_set_ps1(max_val);

    for(unsigned int i = 0; i < sse_iters; i++)
        {
            inputVal1 = _mm_load_ps((float*)inputVectorPtr); inputVectorPtr += 4;
            inputVal2 = _mm_load_ps((float*)inputVectorPtr); inputVectorPtr += 4;
            inputVal3 = _mm_load_ps((float*)inputVectorPtr); inputVectorPtr += 4;
            inputVal4 = _mm_load_ps((float*)inputVectorPtr); inputVectorPtr += 4;

            // Clip
            ret1 = _mm_max_ps(_mm_min_ps(inputVal1, vmax_val), vmin_val);
            ret2 = _mm_max_ps(_mm_min_ps(inputVal2, vmax_val), vmin_val);
            ret3 = _mm_max_ps(_mm_min_ps(inputVal3, vmax_val), vmin_val);
            ret4 = _mm_max_ps(_mm_min_ps(inputVal4, vmax_val), vmin_val);

            intInputVal1 = _mm_cvtps_epi32(ret1);
            intInputVal2 = _mm_cvtps_epi32(ret2);
            intInputVal3 = _mm_cvtps_epi32(ret3);
            intInputVal4 = _mm_cvtps_epi32(ret4);

            intInputVal1 = _mm_packs_epi32(intInputVal1, intInputVal2);
            intInputVal2 = _mm_packs_epi32(intInputVal3, intInputVal4);
            int8InputVal = _mm_packs_epi16(intInputVal1, intInputVal2);

            _mm_store_si128((__m128i*)outputVectorPtr, int8InputVal);
            outputVectorPtr += 16;
        }

    for(unsigned int i = sse_iters * 16; i < num_points * 2; i++)
        {
            if(inputVectorPtr[i] > max_val)
                inputVectorPtr[i] = max_val;
            else if(inputVectorPtr[i] < min_val)
                inputVectorPtr[i] = min_val;
            *outputVectorPtr++ = (int8_t)rintf(*inputVectorPtr++);
        }
}
#endif /* LV_HAVE_SSE2 */

#endif /* INCLUDED_volk_gnsssdr_32fc_convert_8ic_H */
