#include <stdlib.h>    // -*- tab-width:2 coding: viscii mode: c++ -*-
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "config.h"
#include <spell.h>
#include <sentence.h>
#include <wordnode.h>
#include <pfs.h>
#include <glib.h>
using namespace std;

/*
static char *viscii_str = "áàäãÕâ¤¥¦ç§å¡¢ÆÇ£éèë¨©êª«¬­®íìïî¸óòöõ÷ô¯°±²µ½¾¶·ÞþúùüûøßÑ×ØæñýÏÖÛÜðÁÀÄÃÂÅÉÈËÊÍÌÎÓÒ Ô´³ÚÙ¿º»¼ÿ¹ÝÐ";
static gunichar unicode_str[] = {
	 225, 224,7843, 227,7841,
	 226,7845,7847,7849,7851,7853,
	 259,7855,7857,7859,7861,7863,
	 233, 232,7867,7869,7865,
	 234,7871,7873,7875,7877,
	7879, 237, 236,7881, 297,7883,
	 243, 242,7887, 245,7885,
	 244,7889,7891,7893,7895,
	7897, 417,7899,7901,7903,7905,
	7907, 250, 249,7911, 361,7909,
	 432,7913,7915,7917,7919,
	7921, 253,7923,7927,7929,7925,
	 273,
	 193, 192,7842, 195,7840,
	 194,7844,7846,7848,7850,7852,
	 258,7854,7856,7858,7860,7862,
	 201, 200,7866,7868,7864,
	 202,7870,7872,7874,7876,7878,
	 205, 204,7880, 296,7882,
	 211, 210,7886, 213,7884,
	 212,7888,7890,7892,7894,
	7896, 416,7898,7900,7902,7904,
	7906, 218, 217,7910, 360,7908,
	 431,7912,7914,7916,7918,7920,
	 221,7922,7926,7928,7924,
	 272,
	0
};

void viet_utf8_to_viscii(const gchar *in,char *out) // pre-allocated
{
	const gchar *p = in;
	gunichar ch;
	unsigned i,n = strlen(viscii_str);
	while ((ch = g_utf8_get_char(p)) != 0) {
		p = g_utf8_next_char(p);
		if (ch < 128) {
			*out++ = ch;
			continue;
		}
		for (i = 0;i < n;i ++)
			if (unicode_str[i] == ch) {
				*out++ = viscii_str[i];
				break;
			}

		if (i >= n) {
			fprintf(stderr,"Warning: unexpected unicode character %d",ch);
			*out++ = (unsigned char)ch;
		}
	}
	*out = 0;
}

void viet_viscii_to_utf8(const char *in,gchar *out) // pre-allocated
{
	unsigned char *p = (unsigned char*)in;
	unsigned char ch;
	unsigned i,n = strlen(viscii_str);
	while ((ch = *p) != 0) {
		p++;
		if (ch < 128) {
			*out++ = ch;
			continue;
		}
		for (i = 0;i < n;i ++)
			if ((unsigned char)viscii_str[i] == ch) {
				g_unichar_to_utf8(unicode_str[i],out);
				out = g_utf8_next_char(out);
				break;
			}

		if (i >= n) {
			fprintf(stderr,"Warning: unexpected viscii character %d",ch);
			g_unichar_to_utf8(ch,out);
			out = g_utf8_next_char(out);
		}
	}
	*out = 0;
}

char* viet_to_viscii(const char *in)
{
	static char buffer[1000];
	if (g_utf8_strlen(in,-1) >= 1000)
		return "";
	viet_utf8_to_viscii(in,buffer);
	return buffer;
}

char* viet_to_utf8(const char *in)
{
	static char buffer[6000];
	if (strlen(in) >= 1000)
		return "";
	viet_viscii_to_utf8(in,buffer);
	return buffer;
}
*/
