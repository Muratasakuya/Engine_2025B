#pragma once

//============================================================================
//	ParticleConfig
//============================================================================

static const int kMaxGPUParticles = 0xffff;
static const int kMaxCPUParticles = 0x3000;
static const int THREAD_EMIT_GROUP = 1024;
static const int THREAD_UPDATE_GROUP = 1024;