/* alsa_pcm_util.h
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#ifndef _ALSA_PCM_UTIL_H_
#define _ALSA_PCM_UTIL_H_

#include <tinyalsa/asoundlib.h>
#include <sound/asound.h>

#define PCM_ERROR_MAX 128

struct pcm {
  int fd;
  unsigned int flags;
  int running : 1;
  int prepared : 1;
  int underruns;
  unsigned int buffer_size;
  unsigned int boundary;
  char error[PCM_ERROR_MAX];
  struct pcm_config config;
  struct snd_pcm_mmap_status *mmap_status;
  struct snd_pcm_mmap_control *mmap_control;
  struct snd_pcm_sync_ptr *sync_ptr;
  void *mmap_buffer;
  unsigned int noirq_frames_per_msec;
  int wait_for_avail_min;
};

int pcm_set_samplerate(struct pcm *pcm, unsigned int flags,
                       struct pcm_config *config, unsigned short samplerate);
#endif