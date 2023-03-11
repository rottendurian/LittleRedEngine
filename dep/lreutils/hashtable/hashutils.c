#ifndef hashutils_c
#define hashutils_c

#include <stddef.h>
#include <stdint.h>

// size_t hashtable_internal_primes[] = {31, 53, 79, 109, 149, 197, 257, 337, 433, 557, 709, 907, 1151, 1451, 1831, 2309, 2897, 3637, 4561, 5717, 7159, 8963, 11239, 14071, 17599, 22013, 27527, 34421, 43037, 53813, 67289, 84127, 105173, 131477, 164357, 205463, 256873, 321109, 401407, 501769, 627227, 784061, 980107, 1225153, 1531469, 1914361, 2392967, 2991229, 3739051, 4673833, 5842327, 7302929, 9128681, 11410871, 14263603, 17829521, 22286917, 27858659, 34823353, 43529207, 54411523, 68014459, 85018091, 106272637, 132840809, 166051027, 207563831, 259454809, 324318523, 405398177, 506747737, 633434699, 791793391, 989741759, 1237177289, 1546471643, 1933089577, 2416362001, 3020452531, 3775565687, 4719457163, 5899321469, 7374151859, 9217689853, 11522112347, 14402640463, 18003300619, 22504125803, 28130157371, 35162696729, 43953370927, 54941713669, 68677142119, 85846427659, 107308034609, 134135043293, 167668804133, 209586005179, 261982506517, 327478133159, 409347666491, 511684583173, 639605729069, 799507161353, 999383951717, 1249229939689, 1561537424633, 1951921780843, 2439902226067, 3049877782661, 3812347228433, 4765434035591, 5956792544551, 7445990680699, 9307488350911, 11634360438691, 14542950548381, 18178688185583, 22723360231991, 28404200290001, 35505250362539, 44381562953213, 55476953691539, 69346192114547, 86682740143211, 108353425179049, 135441781473877, 169302226842367, 211627783552979, 264534729441293, 330668411801677, 413335514752129, 516669393440177, 645836741800333, 807295927250429, 1009119909063053, 1261399886328851, 1576749857911097, 1970937322388927, 2463671652986177, 3079589566232801, 3849486957791071, 4811858697238861, 6014823371548631, 7518529214435819, 9398161518044813, 11747701897556029, 14684627371945073, 18355784214931363, 22944730268664223, 28680912835830289, 35851141044787877, 44813926305984883, 56017407882481151, 70021759853101489, 87527199816376897, 109408999770471229, 136761249713089129, 170951562141361427, 213689452676701817, 267111815845877377, 333889769807346749, 417362212259183473, 521702765323979387, 652128456654974287, 815160570818718001, 1018950713523397547, 1273688391904246961, 1592110489880308721, 1990138112350385993, 2487672640437982507, 3109590800547478147, 3886988500684347719, 4858735625855434667, 6073419532319293369, 7591774415399116729, 9489718019248895999, 11862147524061120047, 14827684405076400079}; 
size_t hashtable_internal_primes[] = {7,17,31, 79, 149, 257, 433, 709, 1151, 1831, 2897, 4561, 7159, 11239, 17599, 27527, 43037, 67289, 105173, 164357, 256873, 401407, 627227, 980107, 1531469, 2392967, 3739051, 5842327, 9128681, 14263603, 22286917, 34823353, 54411523, 85018091, 132840809, 207563831, 324318523, 506747737, 791793391, 1237177289, 1933089577, 3020452531, 4719457163, 7374151859, 11522112347, 18003300619, 28130157371, 43953370927, 68677142119, 107308034609, 167668804133, 261982506517, 409347666491, 639605729069, 999383951717, 1561537424633, 2439902226067, 3812347228433, 5956792544551, 9307488350911, 14542950548381, 22723360231991, 35505250362539, 55476953691539, 86682740143211, 135441781473877, 211627783552979, 330668411801677, 516669393440177, 807295927250429, 1261399886328851, 1970937322388927, 3079589566232801, 4811858697238861, 7518529214435819, 11747701897556029, 18355784214931363, 28680912835830289, 44813926305984883, 70021759853101489, 109408999770471229, 170951562141361427, 267111815845877377, 417362212259183473, 652128456654974287, 1018950713523397547, 1592110489880308721, 2487672640437982507, 3886988500684347719, 6073419532319293369, 9489718019248895999, 14827684405076400079};




#endif