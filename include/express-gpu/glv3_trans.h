

#ifndef GLV3_TRANS_H
#define GLV3_TRANS_H

#include "direct-express/direct_express_distribute.h"
#include "direct-express/express_device_common.h"
#include "express-gpu/offscreen_render_thread.h"

#define MAX_OUT_BUF_LEN 4096
#define FUNID_glClientWaitSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 1)

#define PARA_NUM_MIN_glClientWaitSync (2)

#define FUNID_glTestInt1 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 2)

#define PARA_NUM_MIN_glTestInt1 (2)

#define FUNID_glTestInt2 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 3)

#define PARA_NUM_MIN_glTestInt2 (2)

#define FUNID_glTestInt3 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 4)

#define PARA_NUM_MIN_glTestInt3 (2)

#define FUNID_glTestInt4 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 5)

#define PARA_NUM_MIN_glTestInt4 (2)

#define FUNID_glTestInt5 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 6)

#define PARA_NUM_MIN_glTestInt5 (2)

#define FUNID_glTestInt6 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 7)

#define PARA_NUM_MIN_glTestInt6 (2)

#define FUNID_glTestPointer1 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 8)

#define PARA_NUM_MIN_glTestPointer1 (2)

#define FUNID_glTestPointer2 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 9)

#define PARA_NUM_MIN_glTestPointer2 (3)

#define FUNID_glTestPointer4 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 10)

#define PARA_NUM_MIN_glTestPointer4 (3)

#define FUNID_glTestString ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 11)

#define PARA_NUM_MIN_glTestString (3)

#define FUNID_glIsBuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 12)

#define PARA_NUM_MIN_glIsBuffer (0)

#define FUNID_glIsEnabled ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 13)

#define PARA_NUM_MIN_glIsEnabled (0)

#define FUNID_glIsFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 14)

#define PARA_NUM_MIN_glIsFramebuffer (0)

#define FUNID_glIsProgram ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 15)

#define PARA_NUM_MIN_glIsProgram (0)

#define FUNID_glIsRenderbuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 16)

#define PARA_NUM_MIN_glIsRenderbuffer (0)

#define FUNID_glIsShader ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 17)

#define PARA_NUM_MIN_glIsShader (0)

#define FUNID_glIsTexture ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 18)

#define PARA_NUM_MIN_glIsTexture (0)

#define FUNID_glIsQuery ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 19)

#define PARA_NUM_MIN_glIsQuery (0)

#define FUNID_glIsVertexArray ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 20)

#define PARA_NUM_MIN_glIsVertexArray (0)

#define FUNID_glIsSampler ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 21)

#define PARA_NUM_MIN_glIsSampler (0)

#define FUNID_glIsTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 22)

#define PARA_NUM_MIN_glIsTransformFeedback (0)

#define FUNID_glIsSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 23)

#define PARA_NUM_MIN_glIsSync (0)

#define FUNID_glGetError ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 24)

#define PARA_NUM_MIN_glGetError (1)

#define FUNID_glGetString_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 25)

#define PARA_NUM_MIN_glGetString_special (2)

#define FUNID_glGetStringi_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 26)

#define PARA_NUM_MIN_glGetStringi_special (2)

#define FUNID_glCheckFramebufferStatus ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 27)

#define PARA_NUM_MIN_glCheckFramebufferStatus (2)

#define FUNID_glQueryMatrixxOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 28)

#define PARA_NUM_MIN_glQueryMatrixxOES (1)

#define FUNID_glGetFramebufferAttachmentParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 29)

#define PARA_NUM_MIN_glGetFramebufferAttachmentParameteriv (2)

#define FUNID_glGetProgramInfoLog ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 30)

#define PARA_NUM_MIN_glGetProgramInfoLog (2)

#define FUNID_glGetRenderbufferParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 31)

#define PARA_NUM_MIN_glGetRenderbufferParameteriv (2)

#define FUNID_glGetShaderInfoLog ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 32)

#define PARA_NUM_MIN_glGetShaderInfoLog (2)

#define FUNID_glGetShaderPrecisionFormat ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 33)

#define PARA_NUM_MIN_glGetShaderPrecisionFormat (2)

#define FUNID_glGetShaderSource ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 34)

#define PARA_NUM_MIN_glGetShaderSource (2)

#define FUNID_glGetTexParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 35)

#define PARA_NUM_MIN_glGetTexParameterfv (2)

#define FUNID_glGetTexParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 36)

#define PARA_NUM_MIN_glGetTexParameteriv (2)

#define FUNID_glGetQueryiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 37)

#define PARA_NUM_MIN_glGetQueryiv (2)

#define FUNID_glGetQueryObjectuiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 38)

#define PARA_NUM_MIN_glGetQueryObjectuiv (2)

#define FUNID_glGetTransformFeedbackVarying ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 39)

#define PARA_NUM_MIN_glGetTransformFeedbackVarying (2)

#define FUNID_glGetActiveUniformsiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 40)

#define PARA_NUM_MIN_glGetActiveUniformsiv (3)

#define FUNID_glGetActiveUniformBlockiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 41)

#define PARA_NUM_MIN_glGetActiveUniformBlockiv (2)

#define FUNID_glGetActiveUniformBlockName ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 42)

#define PARA_NUM_MIN_glGetActiveUniformBlockName (2)

#define FUNID_glGetSamplerParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 43)

#define PARA_NUM_MIN_glGetSamplerParameteriv (2)

#define FUNID_glGetSamplerParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 44)

#define PARA_NUM_MIN_glGetSamplerParameterfv (2)

#define FUNID_glGetProgramBinary ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 45)

#define PARA_NUM_MIN_glGetProgramBinary (2)

#define FUNID_glGetInternalformativ ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 46)

#define PARA_NUM_MIN_glGetInternalformativ (2)

#define FUNID_glGetClipPlanexOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 47)

#define PARA_NUM_MIN_glGetClipPlanexOES (2)

#define FUNID_glGetFixedvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 48)

#define PARA_NUM_MIN_glGetFixedvOES (2)

#define FUNID_glGetTexEnvxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 49)

#define PARA_NUM_MIN_glGetTexEnvxvOES (2)

#define FUNID_glGetTexParameterxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 50)

#define PARA_NUM_MIN_glGetTexParameterxvOES (2)

#define FUNID_glGetLightxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 51)

#define PARA_NUM_MIN_glGetLightxvOES (2)

#define FUNID_glGetMaterialxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 52)

#define PARA_NUM_MIN_glGetMaterialxvOES (2)

#define FUNID_glGetTexGenxvOES ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 53)

#define PARA_NUM_MIN_glGetTexGenxvOES (2)

#define FUNID_glGetFramebufferParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 54)

#define PARA_NUM_MIN_glGetFramebufferParameteriv (2)

#define FUNID_glGetProgramInterfaceiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 55)

#define PARA_NUM_MIN_glGetProgramInterfaceiv (2)

#define FUNID_glGetProgramResourceName ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 56)

#define PARA_NUM_MIN_glGetProgramResourceName (2)

#define FUNID_glGetProgramResourceiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 57)

#define PARA_NUM_MIN_glGetProgramResourceiv (3)

#define FUNID_glGetProgramPipelineiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 58)

#define PARA_NUM_MIN_glGetProgramPipelineiv (2)

#define FUNID_glGetProgramPipelineInfoLog ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 59)

#define PARA_NUM_MIN_glGetProgramPipelineInfoLog (2)

#define FUNID_glGetMultisamplefv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 60)

#define PARA_NUM_MIN_glGetMultisamplefv (2)

#define FUNID_glGetTexLevelParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 61)

#define PARA_NUM_MIN_glGetTexLevelParameteriv (2)

#define FUNID_glGetTexLevelParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 62)

#define PARA_NUM_MIN_glGetTexLevelParameterfv (2)

#define FUNID_glGetSynciv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 63)

#define PARA_NUM_MIN_glGetSynciv (2)

#define FUNID_glGetAttribLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 64)

#define PARA_NUM_MIN_glGetAttribLocation (3)

#define FUNID_glGetUniformLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 65)

#define PARA_NUM_MIN_glGetUniformLocation (3)

#define FUNID_glGetFragDataLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 66)

#define PARA_NUM_MIN_glGetFragDataLocation (3)

#define FUNID_glGetUniformBlockIndex ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 67)

#define PARA_NUM_MIN_glGetUniformBlockIndex (3)

#define FUNID_glGetProgramResourceIndex ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 68)

#define PARA_NUM_MIN_glGetProgramResourceIndex (3)

#define FUNID_glGetProgramResourceLocation ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 69)

#define PARA_NUM_MIN_glGetProgramResourceLocation (3)

#define FUNID_glGetActiveAttrib ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 70)

#define PARA_NUM_MIN_glGetActiveAttrib (2)

#define FUNID_glGetActiveUniform ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 71)

#define PARA_NUM_MIN_glGetActiveUniform (2)

#define FUNID_glGetAttachedShaders ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 72)

#define PARA_NUM_MIN_glGetAttachedShaders (2)

#define FUNID_glGetProgramiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 73)

#define PARA_NUM_MIN_glGetProgramiv (2)

#define FUNID_glGetShaderiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 74)

#define PARA_NUM_MIN_glGetShaderiv (2)

#define FUNID_glGetUniformfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 75)

#define PARA_NUM_MIN_glGetUniformfv (2)

#define FUNID_glGetUniformiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 76)

#define PARA_NUM_MIN_glGetUniformiv (2)

#define FUNID_glGetUniformuiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 77)

#define PARA_NUM_MIN_glGetUniformuiv (2)

#define FUNID_glGetUniformIndices ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 78)

#define PARA_NUM_MIN_glGetUniformIndices (3)

#define FUNID_glGetVertexAttribfv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 79)

#define PARA_NUM_MIN_glGetVertexAttribfv (2)

#define FUNID_glGetVertexAttribiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 80)

#define PARA_NUM_MIN_glGetVertexAttribiv (2)

#define FUNID_glGetVertexAttribIiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 81)

#define PARA_NUM_MIN_glGetVertexAttribIiv (2)

#define FUNID_glGetVertexAttribIuiv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 82)

#define PARA_NUM_MIN_glGetVertexAttribIuiv (2)

#define FUNID_glGetBufferParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 83)

#define PARA_NUM_MIN_glGetBufferParameteriv (2)

#define FUNID_glGetBufferParameteri64v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 84)

#define PARA_NUM_MIN_glGetBufferParameteri64v (2)

#define FUNID_glGetBooleanv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 85)

#define PARA_NUM_MIN_glGetBooleanv (2)

#define FUNID_glGetBooleani_v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 86)

#define PARA_NUM_MIN_glGetBooleani_v (2)

#define FUNID_glGetFloatv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 87)

#define PARA_NUM_MIN_glGetFloatv (2)

#define FUNID_glGetIntegerv ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 88)

#define PARA_NUM_MIN_glGetIntegerv (2)

#define FUNID_glGetIntegeri_v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 89)

#define PARA_NUM_MIN_glGetIntegeri_v (2)

#define FUNID_glGetInteger64v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 90)

#define PARA_NUM_MIN_glGetInteger64v (2)

#define FUNID_glGetInteger64i_v ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 91)

#define PARA_NUM_MIN_glGetInteger64i_v (2)

#define FUNID_glMapBufferRange_read ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 92)

#define PARA_NUM_MIN_glMapBufferRange_read (2)

#define FUNID_glReadPixels_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 93)

#define PARA_NUM_MIN_glReadPixels_without_bound (2)

#define FUNID_glTestPointer3 ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 94)

#define PARA_NUM_MIN_glTestPointer3 (4)

#define FUNID_glFlush ((EXPRESS_GPU_FUN_ID << 32u) + 95)

#define PARA_NUM_MIN_glFlush (0)

#define FUNID_glFinish ((EXPRESS_GPU_FUN_ID << 32u) + 96)

#define PARA_NUM_MIN_glFinish (0)

#define FUNID_glBeginQuery ((EXPRESS_GPU_FUN_ID << 32u) + 97)

#define PARA_NUM_MIN_glBeginQuery (1)

#define FUNID_glEndQuery ((EXPRESS_GPU_FUN_ID << 32u) + 98)

#define PARA_NUM_MIN_glEndQuery (1)

#define FUNID_glViewport ((EXPRESS_GPU_FUN_ID << 32u) + 99)

#define PARA_NUM_MIN_glViewport (1)

#define FUNID_glTexStorage2D ((EXPRESS_GPU_FUN_ID << 32u) + 100)

#define PARA_NUM_MIN_glTexStorage2D (1)

#define FUNID_glTexStorage3D ((EXPRESS_GPU_FUN_ID << 32u) + 101)

#define PARA_NUM_MIN_glTexStorage3D (1)

#define FUNID_glTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 102)

#define PARA_NUM_MIN_glTexImage2D_with_bound (1)

#define FUNID_glTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 103)

#define PARA_NUM_MIN_glTexSubImage2D_with_bound (1)

#define FUNID_glTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 104)

#define PARA_NUM_MIN_glTexImage3D_with_bound (1)

#define FUNID_glTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 105)

#define PARA_NUM_MIN_glTexSubImage3D_with_bound (1)

#define FUNID_glReadPixels_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 106)

#define PARA_NUM_MIN_glReadPixels_with_bound (1)

#define FUNID_glCompressedTexImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 107)

#define PARA_NUM_MIN_glCompressedTexImage2D_with_bound (1)

#define FUNID_glCompressedTexSubImage2D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 108)

#define PARA_NUM_MIN_glCompressedTexSubImage2D_with_bound (1)

#define FUNID_glCompressedTexImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 109)

#define PARA_NUM_MIN_glCompressedTexImage3D_with_bound (1)

#define FUNID_glCompressedTexSubImage3D_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 110)

#define PARA_NUM_MIN_glCompressedTexSubImage3D_with_bound (1)

#define FUNID_glCopyTexImage2D ((EXPRESS_GPU_FUN_ID << 32u) + 111)

#define PARA_NUM_MIN_glCopyTexImage2D (1)

#define FUNID_glCopyTexSubImage2D ((EXPRESS_GPU_FUN_ID << 32u) + 112)

#define PARA_NUM_MIN_glCopyTexSubImage2D (1)

#define FUNID_glCopyTexSubImage3D ((EXPRESS_GPU_FUN_ID << 32u) + 113)

#define PARA_NUM_MIN_glCopyTexSubImage3D (1)

#define FUNID_glVertexAttribPointer_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 114)

#define PARA_NUM_MIN_glVertexAttribPointer_with_bound (1)

#define FUNID_glVertexAttribPointer_offset ((EXPRESS_GPU_FUN_ID << 32u) + 115)

#define PARA_NUM_MIN_glVertexAttribPointer_offset (1)

#define FUNID_glMapBufferRange_write ((EXPRESS_GPU_FUN_ID << 32u) + 116)

#define PARA_NUM_MIN_glMapBufferRange_write (1)

#define FUNID_glUnmapBuffer_special ((EXPRESS_GPU_FUN_ID << 32u) + 117)

#define PARA_NUM_MIN_glUnmapBuffer_special (1)

#define FUNID_glWaitSync ((EXPRESS_GPU_FUN_ID << 32u) + 118)

#define PARA_NUM_MIN_glWaitSync (1)

#define FUNID_glShaderBinary ((EXPRESS_GPU_FUN_ID << 32u) + 119)

#define PARA_NUM_MIN_glShaderBinary (1)

#define FUNID_glProgramBinary_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 120)

#define PARA_NUM_MIN_glProgramBinary_special (2)

#define FUNID_glDrawBuffers ((EXPRESS_GPU_FUN_ID << 32u) + 121)

#define PARA_NUM_MIN_glDrawBuffers (1)

#define FUNID_glDrawArrays_origin ((EXPRESS_GPU_FUN_ID << 32u) + 122)

#define PARA_NUM_MIN_glDrawArrays_origin (1)

#define FUNID_glDrawArraysInstanced_origin ((EXPRESS_GPU_FUN_ID << 32u) + 123)

#define PARA_NUM_MIN_glDrawArraysInstanced_origin (1)

#define FUNID_glDrawElementsInstanced_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 124)

#define PARA_NUM_MIN_glDrawElementsInstanced_with_bound (1)

#define FUNID_glDrawElements_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 125)

#define PARA_NUM_MIN_glDrawElements_with_bound (1)

#define FUNID_glDrawRangeElements_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 126)

#define PARA_NUM_MIN_glDrawRangeElements_with_bound (1)

#define FUNID_glTestIntAsyn ((EXPRESS_GPU_FUN_ID << 32u) + 127)

#define PARA_NUM_MIN_glTestIntAsyn (1)

#define FUNID_glPrintfAsyn ((EXPRESS_GPU_FUN_ID << 32u) + 128)

#define PARA_NUM_MIN_glPrintfAsyn (1)

#define FUNID_glEGLImageTargetTexture2DOES ((EXPRESS_GPU_FUN_ID << 32u) + 129)

#define PARA_NUM_MIN_glEGLImageTargetTexture2DOES (1)

#define FUNID_glEGLImageTargetRenderbufferStorageOES ((EXPRESS_GPU_FUN_ID << 32u) + 130)

#define PARA_NUM_MIN_glEGLImageTargetRenderbufferStorageOES (1)

#define FUNID_glGenBuffers ((EXPRESS_GPU_FUN_ID << 32u) + 131)

#define PARA_NUM_MIN_glGenBuffers (1)

#define FUNID_glGenRenderbuffers ((EXPRESS_GPU_FUN_ID << 32u) + 132)

#define PARA_NUM_MIN_glGenRenderbuffers (1)

#define FUNID_glGenTextures ((EXPRESS_GPU_FUN_ID << 32u) + 133)

#define PARA_NUM_MIN_glGenTextures (1)

#define FUNID_glGenSamplers ((EXPRESS_GPU_FUN_ID << 32u) + 134)

#define PARA_NUM_MIN_glGenSamplers (1)

#define FUNID_glCreateProgram ((EXPRESS_GPU_FUN_ID << 32u) + 135)

#define PARA_NUM_MIN_glCreateProgram (1)

#define FUNID_glCreateShader ((EXPRESS_GPU_FUN_ID << 32u) + 136)

#define PARA_NUM_MIN_glCreateShader (1)

#define FUNID_glFenceSync ((EXPRESS_GPU_FUN_ID << 32u) + 137)

#define PARA_NUM_MIN_glFenceSync (1)

#define FUNID_glCreateShaderProgramv_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 138)

#define PARA_NUM_MIN_glCreateShaderProgramv_special (2)

#define FUNID_glGenFramebuffers ((EXPRESS_GPU_FUN_ID << 32u) + 139)

#define PARA_NUM_MIN_glGenFramebuffers (1)

#define FUNID_glGenProgramPipelines ((EXPRESS_GPU_FUN_ID << 32u) + 140)

#define PARA_NUM_MIN_glGenProgramPipelines (1)

#define FUNID_glGenTransformFeedbacks ((EXPRESS_GPU_FUN_ID << 32u) + 141)

#define PARA_NUM_MIN_glGenTransformFeedbacks (1)

#define FUNID_glGenVertexArrays ((EXPRESS_GPU_FUN_ID << 32u) + 142)

#define PARA_NUM_MIN_glGenVertexArrays (1)

#define FUNID_glGenQueries ((EXPRESS_GPU_FUN_ID << 32u) + 143)

#define PARA_NUM_MIN_glGenQueries (1)

#define FUNID_glDeleteBuffers_origin ((EXPRESS_GPU_FUN_ID << 32u) + 144)

#define PARA_NUM_MIN_glDeleteBuffers_origin (1)

#define FUNID_glDeleteRenderbuffers ((EXPRESS_GPU_FUN_ID << 32u) + 145)

#define PARA_NUM_MIN_glDeleteRenderbuffers (1)

#define FUNID_glDeleteTextures ((EXPRESS_GPU_FUN_ID << 32u) + 146)

#define PARA_NUM_MIN_glDeleteTextures (1)

#define FUNID_glDeleteSamplers ((EXPRESS_GPU_FUN_ID << 32u) + 147)

#define PARA_NUM_MIN_glDeleteSamplers (1)

#define FUNID_glDeleteProgram_origin ((EXPRESS_GPU_FUN_ID << 32u) + 148)

#define PARA_NUM_MIN_glDeleteProgram_origin (1)

#define FUNID_glDeleteShader ((EXPRESS_GPU_FUN_ID << 32u) + 149)

#define PARA_NUM_MIN_glDeleteShader (1)

#define FUNID_glDeleteSync ((EXPRESS_GPU_FUN_ID << 32u) + 150)

#define PARA_NUM_MIN_glDeleteSync (1)

#define FUNID_glDeleteFramebuffers ((EXPRESS_GPU_FUN_ID << 32u) + 151)

#define PARA_NUM_MIN_glDeleteFramebuffers (1)

#define FUNID_glDeleteProgramPipelines ((EXPRESS_GPU_FUN_ID << 32u) + 152)

#define PARA_NUM_MIN_glDeleteProgramPipelines (1)

#define FUNID_glDeleteTransformFeedbacks ((EXPRESS_GPU_FUN_ID << 32u) + 153)

#define PARA_NUM_MIN_glDeleteTransformFeedbacks (1)

#define FUNID_glDeleteVertexArrays_origin ((EXPRESS_GPU_FUN_ID << 32u) + 154)

#define PARA_NUM_MIN_glDeleteVertexArrays_origin (1)

#define FUNID_glDeleteQueries ((EXPRESS_GPU_FUN_ID << 32u) + 155)

#define PARA_NUM_MIN_glDeleteQueries (1)

#define FUNID_glLinkProgram_special ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 156)

#define PARA_NUM_MIN_glLinkProgram_special (2)

#define FUNID_glPixelStorei_origin ((EXPRESS_GPU_FUN_ID << 32u) + 157)

#define PARA_NUM_MIN_glPixelStorei_origin (1)

#define FUNID_glDisableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID << 32u) + 158)

#define PARA_NUM_MIN_glDisableVertexAttribArray_origin (1)

#define FUNID_glEnableVertexAttribArray_origin ((EXPRESS_GPU_FUN_ID << 32u) + 159)

#define PARA_NUM_MIN_glEnableVertexAttribArray_origin (1)

#define FUNID_glReadBuffer_special ((EXPRESS_GPU_FUN_ID << 32u) + 160)

#define PARA_NUM_MIN_glReadBuffer_special (1)

#define FUNID_glVertexAttribDivisor_origin ((EXPRESS_GPU_FUN_ID << 32u) + 161)

#define PARA_NUM_MIN_glVertexAttribDivisor_origin (1)

#define FUNID_glShaderSource_special ((EXPRESS_GPU_FUN_ID << 32u) + 162)

#define PARA_NUM_MIN_glShaderSource_special (1)

#define FUNID_glVertexAttribIPointer_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 163)

#define PARA_NUM_MIN_glVertexAttribIPointer_with_bound (1)

#define FUNID_glVertexAttribIPointer_offset ((EXPRESS_GPU_FUN_ID << 32u) + 164)

#define PARA_NUM_MIN_glVertexAttribIPointer_offset (1)

#define FUNID_glBindVertexArray_special ((EXPRESS_GPU_FUN_ID << 32u) + 165)

#define PARA_NUM_MIN_glBindVertexArray_special (1)

#define FUNID_glBindBuffer_origin ((EXPRESS_GPU_FUN_ID << 32u) + 166)

#define PARA_NUM_MIN_glBindBuffer_origin (1)

#define FUNID_glBeginTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 167)

#define PARA_NUM_MIN_glBeginTransformFeedback (1)

#define FUNID_glEndTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 168)

#define PARA_NUM_MIN_glEndTransformFeedback (0)

#define FUNID_glPauseTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 169)

#define PARA_NUM_MIN_glPauseTransformFeedback (0)

#define FUNID_glResumeTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 170)

#define PARA_NUM_MIN_glResumeTransformFeedback (0)

#define FUNID_glBindBufferRange ((EXPRESS_GPU_FUN_ID << 32u) + 171)

#define PARA_NUM_MIN_glBindBufferRange (1)

#define FUNID_glBindBufferBase ((EXPRESS_GPU_FUN_ID << 32u) + 172)

#define PARA_NUM_MIN_glBindBufferBase (1)

#define FUNID_glBindTexture ((EXPRESS_GPU_FUN_ID << 32u) + 173)

#define PARA_NUM_MIN_glBindTexture (1)

#define FUNID_glBindRenderbuffer ((EXPRESS_GPU_FUN_ID << 32u) + 174)

#define PARA_NUM_MIN_glBindRenderbuffer (1)

#define FUNID_glBindSampler ((EXPRESS_GPU_FUN_ID << 32u) + 175)

#define PARA_NUM_MIN_glBindSampler (1)

#define FUNID_glBindFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 176)

#define PARA_NUM_MIN_glBindFramebuffer (1)

#define FUNID_glBindProgramPipeline ((EXPRESS_GPU_FUN_ID << 32u) + 177)

#define PARA_NUM_MIN_glBindProgramPipeline (1)

#define FUNID_glBindTransformFeedback ((EXPRESS_GPU_FUN_ID << 32u) + 178)

#define PARA_NUM_MIN_glBindTransformFeedback (1)

#define FUNID_glActiveTexture ((EXPRESS_GPU_FUN_ID << 32u) + 179)

#define PARA_NUM_MIN_glActiveTexture (1)

#define FUNID_glAttachShader ((EXPRESS_GPU_FUN_ID << 32u) + 180)

#define PARA_NUM_MIN_glAttachShader (1)

#define FUNID_glBlendColor ((EXPRESS_GPU_FUN_ID << 32u) + 181)

#define PARA_NUM_MIN_glBlendColor (1)

#define FUNID_glBlendEquation ((EXPRESS_GPU_FUN_ID << 32u) + 182)

#define PARA_NUM_MIN_glBlendEquation (1)

#define FUNID_glBlendEquationSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 183)

#define PARA_NUM_MIN_glBlendEquationSeparate (1)

#define FUNID_glBlendFunc ((EXPRESS_GPU_FUN_ID << 32u) + 184)

#define PARA_NUM_MIN_glBlendFunc (1)

#define FUNID_glBlendFuncSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 185)

#define PARA_NUM_MIN_glBlendFuncSeparate (1)

#define FUNID_glClear ((EXPRESS_GPU_FUN_ID << 32u) + 186)

#define PARA_NUM_MIN_glClear (1)

#define FUNID_glClearColor ((EXPRESS_GPU_FUN_ID << 32u) + 187)

#define PARA_NUM_MIN_glClearColor (1)

#define FUNID_glClearDepthf ((EXPRESS_GPU_FUN_ID << 32u) + 188)

#define PARA_NUM_MIN_glClearDepthf (1)

#define FUNID_glClearStencil ((EXPRESS_GPU_FUN_ID << 32u) + 189)

#define PARA_NUM_MIN_glClearStencil (1)

#define FUNID_glColorMask ((EXPRESS_GPU_FUN_ID << 32u) + 190)

#define PARA_NUM_MIN_glColorMask (1)

#define FUNID_glCompileShader ((EXPRESS_GPU_FUN_ID << 32u) + 191)

#define PARA_NUM_MIN_glCompileShader (1)

#define FUNID_glCullFace ((EXPRESS_GPU_FUN_ID << 32u) + 192)

#define PARA_NUM_MIN_glCullFace (1)

#define FUNID_glDepthFunc ((EXPRESS_GPU_FUN_ID << 32u) + 193)

#define PARA_NUM_MIN_glDepthFunc (1)

#define FUNID_glDepthMask ((EXPRESS_GPU_FUN_ID << 32u) + 194)

#define PARA_NUM_MIN_glDepthMask (1)

#define FUNID_glDepthRangef ((EXPRESS_GPU_FUN_ID << 32u) + 195)

#define PARA_NUM_MIN_glDepthRangef (1)

#define FUNID_glDetachShader ((EXPRESS_GPU_FUN_ID << 32u) + 196)

#define PARA_NUM_MIN_glDetachShader (1)

#define FUNID_glDisable ((EXPRESS_GPU_FUN_ID << 32u) + 197)

#define PARA_NUM_MIN_glDisable (1)

#define FUNID_glEnable ((EXPRESS_GPU_FUN_ID << 32u) + 198)

#define PARA_NUM_MIN_glEnable (1)

#define FUNID_glFramebufferRenderbuffer ((EXPRESS_GPU_FUN_ID << 32u) + 199)

#define PARA_NUM_MIN_glFramebufferRenderbuffer (1)

#define FUNID_glFramebufferTexture2D ((EXPRESS_GPU_FUN_ID << 32u) + 200)

#define PARA_NUM_MIN_glFramebufferTexture2D (1)

#define FUNID_glFrontFace ((EXPRESS_GPU_FUN_ID << 32u) + 201)

#define PARA_NUM_MIN_glFrontFace (1)

#define FUNID_glGenerateMipmap ((EXPRESS_GPU_FUN_ID << 32u) + 202)

#define PARA_NUM_MIN_glGenerateMipmap (1)

#define FUNID_glHint ((EXPRESS_GPU_FUN_ID << 32u) + 203)

#define PARA_NUM_MIN_glHint (1)

#define FUNID_glLineWidth ((EXPRESS_GPU_FUN_ID << 32u) + 204)

#define PARA_NUM_MIN_glLineWidth (1)

#define FUNID_glPolygonOffset ((EXPRESS_GPU_FUN_ID << 32u) + 205)

#define PARA_NUM_MIN_glPolygonOffset (1)

#define FUNID_glReleaseShaderCompiler ((EXPRESS_GPU_FUN_ID << 32u) + 206)

#define PARA_NUM_MIN_glReleaseShaderCompiler (0)

#define FUNID_glRenderbufferStorage ((EXPRESS_GPU_FUN_ID << 32u) + 207)

#define PARA_NUM_MIN_glRenderbufferStorage (1)

#define FUNID_glSampleCoverage ((EXPRESS_GPU_FUN_ID << 32u) + 208)

#define PARA_NUM_MIN_glSampleCoverage (1)

#define FUNID_glScissor ((EXPRESS_GPU_FUN_ID << 32u) + 209)

#define PARA_NUM_MIN_glScissor (1)

#define FUNID_glStencilFunc ((EXPRESS_GPU_FUN_ID << 32u) + 210)

#define PARA_NUM_MIN_glStencilFunc (1)

#define FUNID_glStencilFuncSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 211)

#define PARA_NUM_MIN_glStencilFuncSeparate (1)

#define FUNID_glStencilMask ((EXPRESS_GPU_FUN_ID << 32u) + 212)

#define PARA_NUM_MIN_glStencilMask (1)

#define FUNID_glStencilMaskSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 213)

#define PARA_NUM_MIN_glStencilMaskSeparate (1)

#define FUNID_glStencilOp ((EXPRESS_GPU_FUN_ID << 32u) + 214)

#define PARA_NUM_MIN_glStencilOp (1)

#define FUNID_glStencilOpSeparate ((EXPRESS_GPU_FUN_ID << 32u) + 215)

#define PARA_NUM_MIN_glStencilOpSeparate (1)

#define FUNID_glTexParameterf ((EXPRESS_GPU_FUN_ID << 32u) + 216)

#define PARA_NUM_MIN_glTexParameterf (1)

#define FUNID_glTexParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 217)

#define PARA_NUM_MIN_glTexParameteri (1)

#define FUNID_glUniform1f ((EXPRESS_GPU_FUN_ID << 32u) + 218)

#define PARA_NUM_MIN_glUniform1f (1)

#define FUNID_glUniform1i ((EXPRESS_GPU_FUN_ID << 32u) + 219)

#define PARA_NUM_MIN_glUniform1i (1)

#define FUNID_glUniform2f ((EXPRESS_GPU_FUN_ID << 32u) + 220)

#define PARA_NUM_MIN_glUniform2f (1)

#define FUNID_glUniform2i ((EXPRESS_GPU_FUN_ID << 32u) + 221)

#define PARA_NUM_MIN_glUniform2i (1)

#define FUNID_glUniform3f ((EXPRESS_GPU_FUN_ID << 32u) + 222)

#define PARA_NUM_MIN_glUniform3f (1)

#define FUNID_glUniform3i ((EXPRESS_GPU_FUN_ID << 32u) + 223)

#define PARA_NUM_MIN_glUniform3i (1)

#define FUNID_glUniform4f ((EXPRESS_GPU_FUN_ID << 32u) + 224)

#define PARA_NUM_MIN_glUniform4f (1)

#define FUNID_glUniform4i ((EXPRESS_GPU_FUN_ID << 32u) + 225)

#define PARA_NUM_MIN_glUniform4i (1)

#define FUNID_glUseProgram ((EXPRESS_GPU_FUN_ID << 32u) + 226)

#define PARA_NUM_MIN_glUseProgram (1)

#define FUNID_glValidateProgram ((EXPRESS_GPU_FUN_ID << 32u) + 227)

#define PARA_NUM_MIN_glValidateProgram (1)

#define FUNID_glVertexAttrib1f ((EXPRESS_GPU_FUN_ID << 32u) + 228)

#define PARA_NUM_MIN_glVertexAttrib1f (1)

#define FUNID_glVertexAttrib2f ((EXPRESS_GPU_FUN_ID << 32u) + 229)

#define PARA_NUM_MIN_glVertexAttrib2f (1)

#define FUNID_glVertexAttrib3f ((EXPRESS_GPU_FUN_ID << 32u) + 230)

#define PARA_NUM_MIN_glVertexAttrib3f (1)

#define FUNID_glVertexAttrib4f ((EXPRESS_GPU_FUN_ID << 32u) + 231)

#define PARA_NUM_MIN_glVertexAttrib4f (1)

#define FUNID_glBlitFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 232)

#define PARA_NUM_MIN_glBlitFramebuffer (1)

#define FUNID_glRenderbufferStorageMultisample ((EXPRESS_GPU_FUN_ID << 32u) + 233)

#define PARA_NUM_MIN_glRenderbufferStorageMultisample (1)

#define FUNID_glFramebufferTextureLayer ((EXPRESS_GPU_FUN_ID << 32u) + 234)

#define PARA_NUM_MIN_glFramebufferTextureLayer (1)

#define FUNID_glVertexAttribI4i ((EXPRESS_GPU_FUN_ID << 32u) + 235)

#define PARA_NUM_MIN_glVertexAttribI4i (1)

#define FUNID_glVertexAttribI4ui ((EXPRESS_GPU_FUN_ID << 32u) + 236)

#define PARA_NUM_MIN_glVertexAttribI4ui (1)

#define FUNID_glUniform1ui ((EXPRESS_GPU_FUN_ID << 32u) + 237)

#define PARA_NUM_MIN_glUniform1ui (1)

#define FUNID_glUniform2ui ((EXPRESS_GPU_FUN_ID << 32u) + 238)

#define PARA_NUM_MIN_glUniform2ui (1)

#define FUNID_glUniform3ui ((EXPRESS_GPU_FUN_ID << 32u) + 239)

#define PARA_NUM_MIN_glUniform3ui (1)

#define FUNID_glUniform4ui ((EXPRESS_GPU_FUN_ID << 32u) + 240)

#define PARA_NUM_MIN_glUniform4ui (1)

#define FUNID_glClearBufferfi ((EXPRESS_GPU_FUN_ID << 32u) + 241)

#define PARA_NUM_MIN_glClearBufferfi (1)

#define FUNID_glCopyBufferSubData ((EXPRESS_GPU_FUN_ID << 32u) + 242)

#define PARA_NUM_MIN_glCopyBufferSubData (1)

#define FUNID_glUniformBlockBinding ((EXPRESS_GPU_FUN_ID << 32u) + 243)

#define PARA_NUM_MIN_glUniformBlockBinding (1)

#define FUNID_glSamplerParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 244)

#define PARA_NUM_MIN_glSamplerParameteri (1)

#define FUNID_glSamplerParameterf ((EXPRESS_GPU_FUN_ID << 32u) + 245)

#define PARA_NUM_MIN_glSamplerParameterf (1)

#define FUNID_glProgramParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 246)

#define PARA_NUM_MIN_glProgramParameteri (1)

#define FUNID_glAlphaFuncxOES ((EXPRESS_GPU_FUN_ID << 32u) + 247)

#define PARA_NUM_MIN_glAlphaFuncxOES (1)

#define FUNID_glClearColorxOES ((EXPRESS_GPU_FUN_ID << 32u) + 248)

#define PARA_NUM_MIN_glClearColorxOES (1)

#define FUNID_glClearDepthxOES ((EXPRESS_GPU_FUN_ID << 32u) + 249)

#define PARA_NUM_MIN_glClearDepthxOES (1)

#define FUNID_glColor4xOES ((EXPRESS_GPU_FUN_ID << 32u) + 250)

#define PARA_NUM_MIN_glColor4xOES (1)

#define FUNID_glDepthRangexOES ((EXPRESS_GPU_FUN_ID << 32u) + 251)

#define PARA_NUM_MIN_glDepthRangexOES (1)

#define FUNID_glFogxOES ((EXPRESS_GPU_FUN_ID << 32u) + 252)

#define PARA_NUM_MIN_glFogxOES (1)

#define FUNID_glFrustumxOES ((EXPRESS_GPU_FUN_ID << 32u) + 253)

#define PARA_NUM_MIN_glFrustumxOES (1)

#define FUNID_glLightModelxOES ((EXPRESS_GPU_FUN_ID << 32u) + 254)

#define PARA_NUM_MIN_glLightModelxOES (1)

#define FUNID_glLightxOES ((EXPRESS_GPU_FUN_ID << 32u) + 255)

#define PARA_NUM_MIN_glLightxOES (1)

#define FUNID_glLineWidthxOES ((EXPRESS_GPU_FUN_ID << 32u) + 256)

#define PARA_NUM_MIN_glLineWidthxOES (1)

#define FUNID_glMaterialxOES ((EXPRESS_GPU_FUN_ID << 32u) + 257)

#define PARA_NUM_MIN_glMaterialxOES (1)

#define FUNID_glMultiTexCoord4xOES ((EXPRESS_GPU_FUN_ID << 32u) + 258)

#define PARA_NUM_MIN_glMultiTexCoord4xOES (1)

#define FUNID_glNormal3xOES ((EXPRESS_GPU_FUN_ID << 32u) + 259)

#define PARA_NUM_MIN_glNormal3xOES (1)

#define FUNID_glOrthoxOES ((EXPRESS_GPU_FUN_ID << 32u) + 260)

#define PARA_NUM_MIN_glOrthoxOES (1)

#define FUNID_glPointSizexOES ((EXPRESS_GPU_FUN_ID << 32u) + 261)

#define PARA_NUM_MIN_glPointSizexOES (1)

#define FUNID_glPolygonOffsetxOES ((EXPRESS_GPU_FUN_ID << 32u) + 262)

#define PARA_NUM_MIN_glPolygonOffsetxOES (1)

#define FUNID_glRotatexOES ((EXPRESS_GPU_FUN_ID << 32u) + 263)

#define PARA_NUM_MIN_glRotatexOES (1)

#define FUNID_glScalexOES ((EXPRESS_GPU_FUN_ID << 32u) + 264)

#define PARA_NUM_MIN_glScalexOES (1)

#define FUNID_glTexEnvxOES ((EXPRESS_GPU_FUN_ID << 32u) + 265)

#define PARA_NUM_MIN_glTexEnvxOES (1)

#define FUNID_glTranslatexOES ((EXPRESS_GPU_FUN_ID << 32u) + 266)

#define PARA_NUM_MIN_glTranslatexOES (1)

#define FUNID_glPointParameterxOES ((EXPRESS_GPU_FUN_ID << 32u) + 267)

#define PARA_NUM_MIN_glPointParameterxOES (1)

#define FUNID_glSampleCoveragexOES ((EXPRESS_GPU_FUN_ID << 32u) + 268)

#define PARA_NUM_MIN_glSampleCoveragexOES (1)

#define FUNID_glTexGenxOES ((EXPRESS_GPU_FUN_ID << 32u) + 269)

#define PARA_NUM_MIN_glTexGenxOES (1)

#define FUNID_glClearDepthfOES ((EXPRESS_GPU_FUN_ID << 32u) + 270)

#define PARA_NUM_MIN_glClearDepthfOES (1)

#define FUNID_glDepthRangefOES ((EXPRESS_GPU_FUN_ID << 32u) + 271)

#define PARA_NUM_MIN_glDepthRangefOES (1)

#define FUNID_glFrustumfOES ((EXPRESS_GPU_FUN_ID << 32u) + 272)

#define PARA_NUM_MIN_glFrustumfOES (1)

#define FUNID_glOrthofOES ((EXPRESS_GPU_FUN_ID << 32u) + 273)

#define PARA_NUM_MIN_glOrthofOES (1)

#define FUNID_glRenderbufferStorageMultisampleEXT ((EXPRESS_GPU_FUN_ID << 32u) + 274)

#define PARA_NUM_MIN_glRenderbufferStorageMultisampleEXT (1)

#define FUNID_glUseProgramStages ((EXPRESS_GPU_FUN_ID << 32u) + 275)

#define PARA_NUM_MIN_glUseProgramStages (1)

#define FUNID_glActiveShaderProgram ((EXPRESS_GPU_FUN_ID << 32u) + 276)

#define PARA_NUM_MIN_glActiveShaderProgram (1)

#define FUNID_glProgramUniform1i ((EXPRESS_GPU_FUN_ID << 32u) + 277)

#define PARA_NUM_MIN_glProgramUniform1i (1)

#define FUNID_glProgramUniform2i ((EXPRESS_GPU_FUN_ID << 32u) + 278)

#define PARA_NUM_MIN_glProgramUniform2i (1)

#define FUNID_glProgramUniform3i ((EXPRESS_GPU_FUN_ID << 32u) + 279)

#define PARA_NUM_MIN_glProgramUniform3i (1)

#define FUNID_glProgramUniform4i ((EXPRESS_GPU_FUN_ID << 32u) + 280)

#define PARA_NUM_MIN_glProgramUniform4i (1)

#define FUNID_glProgramUniform1ui ((EXPRESS_GPU_FUN_ID << 32u) + 281)

#define PARA_NUM_MIN_glProgramUniform1ui (1)

#define FUNID_glProgramUniform2ui ((EXPRESS_GPU_FUN_ID << 32u) + 282)

#define PARA_NUM_MIN_glProgramUniform2ui (1)

#define FUNID_glProgramUniform3ui ((EXPRESS_GPU_FUN_ID << 32u) + 283)

#define PARA_NUM_MIN_glProgramUniform3ui (1)

#define FUNID_glProgramUniform4ui ((EXPRESS_GPU_FUN_ID << 32u) + 284)

#define PARA_NUM_MIN_glProgramUniform4ui (1)

#define FUNID_glProgramUniform1f ((EXPRESS_GPU_FUN_ID << 32u) + 285)

#define PARA_NUM_MIN_glProgramUniform1f (1)

#define FUNID_glProgramUniform2f ((EXPRESS_GPU_FUN_ID << 32u) + 286)

#define PARA_NUM_MIN_glProgramUniform2f (1)

#define FUNID_glProgramUniform3f ((EXPRESS_GPU_FUN_ID << 32u) + 287)

#define PARA_NUM_MIN_glProgramUniform3f (1)

#define FUNID_glProgramUniform4f ((EXPRESS_GPU_FUN_ID << 32u) + 288)

#define PARA_NUM_MIN_glProgramUniform4f (1)

#define FUNID_glTransformFeedbackVaryings ((EXPRESS_GPU_FUN_ID << 32u) + 289)

#define PARA_NUM_MIN_glTransformFeedbackVaryings (1)

#define FUNID_glTexParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + 290)

#define PARA_NUM_MIN_glTexParameterfv (1)

#define FUNID_glTexParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + 291)

#define PARA_NUM_MIN_glTexParameteriv (1)

#define FUNID_glUniform1fv ((EXPRESS_GPU_FUN_ID << 32u) + 292)

#define PARA_NUM_MIN_glUniform1fv (1)

#define FUNID_glUniform1iv ((EXPRESS_GPU_FUN_ID << 32u) + 293)

#define PARA_NUM_MIN_glUniform1iv (1)

#define FUNID_glUniform2fv ((EXPRESS_GPU_FUN_ID << 32u) + 294)

#define PARA_NUM_MIN_glUniform2fv (1)

#define FUNID_glUniform2iv ((EXPRESS_GPU_FUN_ID << 32u) + 295)

#define PARA_NUM_MIN_glUniform2iv (1)

#define FUNID_glUniform3fv ((EXPRESS_GPU_FUN_ID << 32u) + 296)

#define PARA_NUM_MIN_glUniform3fv (1)

#define FUNID_glUniform3iv ((EXPRESS_GPU_FUN_ID << 32u) + 297)

#define PARA_NUM_MIN_glUniform3iv (1)

#define FUNID_glUniform4fv ((EXPRESS_GPU_FUN_ID << 32u) + 298)

#define PARA_NUM_MIN_glUniform4fv (1)

#define FUNID_glUniform4iv ((EXPRESS_GPU_FUN_ID << 32u) + 299)

#define PARA_NUM_MIN_glUniform4iv (1)

#define FUNID_glVertexAttrib1fv ((EXPRESS_GPU_FUN_ID << 32u) + 300)

#define PARA_NUM_MIN_glVertexAttrib1fv (1)

#define FUNID_glVertexAttrib2fv ((EXPRESS_GPU_FUN_ID << 32u) + 301)

#define PARA_NUM_MIN_glVertexAttrib2fv (1)

#define FUNID_glVertexAttrib3fv ((EXPRESS_GPU_FUN_ID << 32u) + 302)

#define PARA_NUM_MIN_glVertexAttrib3fv (1)

#define FUNID_glVertexAttrib4fv ((EXPRESS_GPU_FUN_ID << 32u) + 303)

#define PARA_NUM_MIN_glVertexAttrib4fv (1)

#define FUNID_glUniformMatrix2fv ((EXPRESS_GPU_FUN_ID << 32u) + 304)

#define PARA_NUM_MIN_glUniformMatrix2fv (1)

#define FUNID_glUniformMatrix3fv ((EXPRESS_GPU_FUN_ID << 32u) + 305)

#define PARA_NUM_MIN_glUniformMatrix3fv (1)

#define FUNID_glUniformMatrix4fv ((EXPRESS_GPU_FUN_ID << 32u) + 306)

#define PARA_NUM_MIN_glUniformMatrix4fv (1)

#define FUNID_glUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 307)

#define PARA_NUM_MIN_glUniformMatrix2x3fv (1)

#define FUNID_glUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 308)

#define PARA_NUM_MIN_glUniformMatrix3x2fv (1)

#define FUNID_glUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 309)

#define PARA_NUM_MIN_glUniformMatrix2x4fv (1)

#define FUNID_glUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 310)

#define PARA_NUM_MIN_glUniformMatrix4x2fv (1)

#define FUNID_glUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 311)

#define PARA_NUM_MIN_glUniformMatrix3x4fv (1)

#define FUNID_glUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 312)

#define PARA_NUM_MIN_glUniformMatrix4x3fv (1)

#define FUNID_glVertexAttribI4iv ((EXPRESS_GPU_FUN_ID << 32u) + 313)

#define PARA_NUM_MIN_glVertexAttribI4iv (1)

#define FUNID_glVertexAttribI4uiv ((EXPRESS_GPU_FUN_ID << 32u) + 314)

#define PARA_NUM_MIN_glVertexAttribI4uiv (1)

#define FUNID_glUniform1uiv ((EXPRESS_GPU_FUN_ID << 32u) + 315)

#define PARA_NUM_MIN_glUniform1uiv (1)

#define FUNID_glUniform2uiv ((EXPRESS_GPU_FUN_ID << 32u) + 316)

#define PARA_NUM_MIN_glUniform2uiv (1)

#define FUNID_glUniform3uiv ((EXPRESS_GPU_FUN_ID << 32u) + 317)

#define PARA_NUM_MIN_glUniform3uiv (1)

#define FUNID_glUniform4uiv ((EXPRESS_GPU_FUN_ID << 32u) + 318)

#define PARA_NUM_MIN_glUniform4uiv (1)

#define FUNID_glClearBufferiv ((EXPRESS_GPU_FUN_ID << 32u) + 319)

#define PARA_NUM_MIN_glClearBufferiv (1)

#define FUNID_glClearBufferuiv ((EXPRESS_GPU_FUN_ID << 32u) + 320)

#define PARA_NUM_MIN_glClearBufferuiv (1)

#define FUNID_glClearBufferfv ((EXPRESS_GPU_FUN_ID << 32u) + 321)

#define PARA_NUM_MIN_glClearBufferfv (1)

#define FUNID_glSamplerParameteriv ((EXPRESS_GPU_FUN_ID << 32u) + 322)

#define PARA_NUM_MIN_glSamplerParameteriv (1)

#define FUNID_glSamplerParameterfv ((EXPRESS_GPU_FUN_ID << 32u) + 323)

#define PARA_NUM_MIN_glSamplerParameterfv (1)

#define FUNID_glInvalidateFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 324)

#define PARA_NUM_MIN_glInvalidateFramebuffer (1)

#define FUNID_glInvalidateSubFramebuffer ((EXPRESS_GPU_FUN_ID << 32u) + 325)

#define PARA_NUM_MIN_glInvalidateSubFramebuffer (1)

#define FUNID_glClipPlanexOES ((EXPRESS_GPU_FUN_ID << 32u) + 326)

#define PARA_NUM_MIN_glClipPlanexOES (1)

#define FUNID_glFogxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 327)

#define PARA_NUM_MIN_glFogxvOES (1)

#define FUNID_glLightModelxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 328)

#define PARA_NUM_MIN_glLightModelxvOES (1)

#define FUNID_glLightxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 329)

#define PARA_NUM_MIN_glLightxvOES (1)

#define FUNID_glLoadMatrixxOES ((EXPRESS_GPU_FUN_ID << 32u) + 330)

#define PARA_NUM_MIN_glLoadMatrixxOES (0)

#define FUNID_glMaterialxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 331)

#define PARA_NUM_MIN_glMaterialxvOES (1)

#define FUNID_glMultMatrixxOES ((EXPRESS_GPU_FUN_ID << 32u) + 332)

#define PARA_NUM_MIN_glMultMatrixxOES (0)

#define FUNID_glPointParameterxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 333)

#define PARA_NUM_MIN_glPointParameterxvOES (1)

#define FUNID_glTexEnvxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 334)

#define PARA_NUM_MIN_glTexEnvxvOES (1)

#define FUNID_glClipPlanefOES ((EXPRESS_GPU_FUN_ID << 32u) + 335)

#define PARA_NUM_MIN_glClipPlanefOES (1)

#define FUNID_glTexGenxvOES ((EXPRESS_GPU_FUN_ID << 32u) + 336)

#define PARA_NUM_MIN_glTexGenxvOES (1)

#define FUNID_glProgramUniform1iv ((EXPRESS_GPU_FUN_ID << 32u) + 337)

#define PARA_NUM_MIN_glProgramUniform1iv (1)

#define FUNID_glProgramUniform2iv ((EXPRESS_GPU_FUN_ID << 32u) + 338)

#define PARA_NUM_MIN_glProgramUniform2iv (1)

#define FUNID_glProgramUniform3iv ((EXPRESS_GPU_FUN_ID << 32u) + 339)

#define PARA_NUM_MIN_glProgramUniform3iv (1)

#define FUNID_glProgramUniform4iv ((EXPRESS_GPU_FUN_ID << 32u) + 340)

#define PARA_NUM_MIN_glProgramUniform4iv (1)

#define FUNID_glProgramUniform1uiv ((EXPRESS_GPU_FUN_ID << 32u) + 341)

#define PARA_NUM_MIN_glProgramUniform1uiv (1)

#define FUNID_glProgramUniform2uiv ((EXPRESS_GPU_FUN_ID << 32u) + 342)

#define PARA_NUM_MIN_glProgramUniform2uiv (1)

#define FUNID_glProgramUniform3uiv ((EXPRESS_GPU_FUN_ID << 32u) + 343)

#define PARA_NUM_MIN_glProgramUniform3uiv (1)

#define FUNID_glProgramUniform4uiv ((EXPRESS_GPU_FUN_ID << 32u) + 344)

#define PARA_NUM_MIN_glProgramUniform4uiv (1)

#define FUNID_glProgramUniform1fv ((EXPRESS_GPU_FUN_ID << 32u) + 345)

#define PARA_NUM_MIN_glProgramUniform1fv (1)

#define FUNID_glProgramUniform2fv ((EXPRESS_GPU_FUN_ID << 32u) + 346)

#define PARA_NUM_MIN_glProgramUniform2fv (1)

#define FUNID_glProgramUniform3fv ((EXPRESS_GPU_FUN_ID << 32u) + 347)

#define PARA_NUM_MIN_glProgramUniform3fv (1)

#define FUNID_glProgramUniform4fv ((EXPRESS_GPU_FUN_ID << 32u) + 348)

#define PARA_NUM_MIN_glProgramUniform4fv (1)

#define FUNID_glProgramUniformMatrix2fv ((EXPRESS_GPU_FUN_ID << 32u) + 349)

#define PARA_NUM_MIN_glProgramUniformMatrix2fv (1)

#define FUNID_glProgramUniformMatrix3fv ((EXPRESS_GPU_FUN_ID << 32u) + 350)

#define PARA_NUM_MIN_glProgramUniformMatrix3fv (1)

#define FUNID_glProgramUniformMatrix4fv ((EXPRESS_GPU_FUN_ID << 32u) + 351)

#define PARA_NUM_MIN_glProgramUniformMatrix4fv (1)

#define FUNID_glProgramUniformMatrix2x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 352)

#define PARA_NUM_MIN_glProgramUniformMatrix2x3fv (1)

#define FUNID_glProgramUniformMatrix3x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 353)

#define PARA_NUM_MIN_glProgramUniformMatrix3x2fv (1)

#define FUNID_glProgramUniformMatrix2x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 354)

#define PARA_NUM_MIN_glProgramUniformMatrix2x4fv (1)

#define FUNID_glProgramUniformMatrix4x2fv ((EXPRESS_GPU_FUN_ID << 32u) + 355)

#define PARA_NUM_MIN_glProgramUniformMatrix4x2fv (1)

#define FUNID_glProgramUniformMatrix3x4fv ((EXPRESS_GPU_FUN_ID << 32u) + 356)

#define PARA_NUM_MIN_glProgramUniformMatrix3x4fv (1)

#define FUNID_glProgramUniformMatrix4x3fv ((EXPRESS_GPU_FUN_ID << 32u) + 357)

#define PARA_NUM_MIN_glProgramUniformMatrix4x3fv (1)

#define FUNID_glBindAttribLocation ((EXPRESS_GPU_FUN_ID << 32u) + 358)

#define PARA_NUM_MIN_glBindAttribLocation (1)

#define FUNID_glTexEnvf ((EXPRESS_GPU_FUN_ID << 32u) + 359)

#define PARA_NUM_MIN_glTexEnvf (1)

#define FUNID_glTexEnvi ((EXPRESS_GPU_FUN_ID << 32u) + 360)

#define PARA_NUM_MIN_glTexEnvi (1)

#define FUNID_glTexEnvx ((EXPRESS_GPU_FUN_ID << 32u) + 361)

#define PARA_NUM_MIN_glTexEnvx (1)

#define FUNID_glTexParameterx ((EXPRESS_GPU_FUN_ID << 32u) + 362)

#define PARA_NUM_MIN_glTexParameterx (1)

#define FUNID_glShadeModel ((EXPRESS_GPU_FUN_ID << 32u) + 363)

#define PARA_NUM_MIN_glShadeModel (1)

#define FUNID_glDrawTexiOES ((EXPRESS_GPU_FUN_ID << 32u) + 364)

#define PARA_NUM_MIN_glDrawTexiOES (1)

#define FUNID_glVertexAttribIPointer_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 365)

#define PARA_NUM_MIN_glVertexAttribIPointer_without_bound (2)

#define FUNID_glVertexAttribPointer_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 366)

#define PARA_NUM_MIN_glVertexAttribPointer_without_bound (2)

#define FUNID_glDrawElements_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 367)

#define PARA_NUM_MIN_glDrawElements_without_bound (2)

#define FUNID_glDrawElementsInstanced_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 368)

#define PARA_NUM_MIN_glDrawElementsInstanced_without_bound (2)

#define FUNID_glDrawRangeElements_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 369)

#define PARA_NUM_MIN_glDrawRangeElements_without_bound (2)

#define FUNID_glFlushMappedBufferRange_special ((EXPRESS_GPU_FUN_ID << 32u) + 370)

#define PARA_NUM_MIN_glFlushMappedBufferRange_special (2)

#define FUNID_glBufferData_custom ((EXPRESS_GPU_FUN_ID << 32u) + 371)

#define PARA_NUM_MIN_glBufferData_custom (2)

#define FUNID_glBufferSubData_custom ((EXPRESS_GPU_FUN_ID << 32u) + 372)

#define PARA_NUM_MIN_glBufferSubData_custom (2)

#define FUNID_glCompressedTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 373)

#define PARA_NUM_MIN_glCompressedTexImage2D_without_bound (2)

#define FUNID_glCompressedTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 374)

#define PARA_NUM_MIN_glCompressedTexSubImage2D_without_bound (2)

#define FUNID_glCompressedTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 375)

#define PARA_NUM_MIN_glCompressedTexImage3D_without_bound (2)

#define FUNID_glCompressedTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 376)

#define PARA_NUM_MIN_glCompressedTexSubImage3D_without_bound (2)

#define FUNID_glTexImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 377)

#define PARA_NUM_MIN_glTexImage2D_without_bound (2)

#define FUNID_glTexImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 378)

#define PARA_NUM_MIN_glTexImage3D_without_bound (2)

#define FUNID_glTexSubImage2D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 379)

#define PARA_NUM_MIN_glTexSubImage2D_without_bound (2)

#define FUNID_glTexSubImage3D_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 380)

#define PARA_NUM_MIN_glTexSubImage3D_without_bound (2)

#define FUNID_glPrintf ((EXPRESS_GPU_FUN_ID << 32u) + 381)

#define PARA_NUM_MIN_glPrintf (2)

#define FUNID_glBindEGLImage ((EXPRESS_GPU_FUN_ID << 32u) + 382)

#define PARA_NUM_MIN_glBindEGLImage (1)

#define FUNID_glGraphicBufferData ((EXPRESS_GPU_FUN_ID << 32u) + 383)

#define PARA_NUM_MIN_glGraphicBufferData (2)

#define FUNID_glReadGraphicBuffer ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 384)

#define PARA_NUM_MIN_glReadGraphicBuffer (2)

#define FUNID_glGetStaticValues ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 385)

#define PARA_NUM_MIN_glGetStaticValues (1)

#define FUNID_glGetProgramData ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 386)

#define PARA_NUM_MIN_glGetProgramData (2)

#define FUNID_glSync ((EXPRESS_GPU_FUN_ID << 32u) + (((unsigned long long)0x1) << 24u) + 387)

#define PARA_NUM_MIN_glSync (0)

#define FUNID_glBindImageTexture ((EXPRESS_GPU_FUN_ID << 32u) + 388)

#define PARA_NUM_MIN_glBindImageTexture (1)

#define FUNID_glBindVertexBuffer ((EXPRESS_GPU_FUN_ID << 32u) + 389)

#define PARA_NUM_MIN_glBindVertexBuffer (1)

#define FUNID_glVertexAttribFormat ((EXPRESS_GPU_FUN_ID << 32u) + 390)

#define PARA_NUM_MIN_glVertexAttribFormat (1)

#define FUNID_glVertexAttribIFormat ((EXPRESS_GPU_FUN_ID << 32u) + 391)

#define PARA_NUM_MIN_glVertexAttribIFormat (1)

#define FUNID_glVertexAttribBinding ((EXPRESS_GPU_FUN_ID << 32u) + 392)

#define PARA_NUM_MIN_glVertexAttribBinding (1)

#define FUNID_glDispatchCompute ((EXPRESS_GPU_FUN_ID << 32u) + 393)

#define PARA_NUM_MIN_glDispatchCompute (1)

#define FUNID_glDispatchComputeIndirect ((EXPRESS_GPU_FUN_ID << 32u) + 394)

#define PARA_NUM_MIN_glDispatchComputeIndirect (1)

#define FUNID_glMemoryBarrier ((EXPRESS_GPU_FUN_ID << 32u) + 395)

#define PARA_NUM_MIN_glMemoryBarrier (1)

#define FUNID_glMemoryBarrierByRegion ((EXPRESS_GPU_FUN_ID << 32u) + 396)

#define PARA_NUM_MIN_glMemoryBarrierByRegion (1)

#define FUNID_glFramebufferParameteri ((EXPRESS_GPU_FUN_ID << 32u) + 397)

#define PARA_NUM_MIN_glFramebufferParameteri (1)

#define FUNID_glSampleMaski ((EXPRESS_GPU_FUN_ID << 32u) + 398)

#define PARA_NUM_MIN_glSampleMaski (1)

#define FUNID_glTexStorage2DMultisample ((EXPRESS_GPU_FUN_ID << 32u) + 399)

#define PARA_NUM_MIN_glTexStorage2DMultisample (1)

#define FUNID_glValidateProgramPipeline ((EXPRESS_GPU_FUN_ID << 32u) + 400)

#define PARA_NUM_MIN_glValidateProgramPipeline (1)

#define FUNID_glVertexBindingDivisor ((EXPRESS_GPU_FUN_ID << 32u) + 401)

#define PARA_NUM_MIN_glVertexBindingDivisor (1)

#define FUNID_glDrawArraysIndirect_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 402)

#define PARA_NUM_MIN_glDrawArraysIndirect_with_bound (1)

#define FUNID_glDrawArraysIndirect_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 403)

#define PARA_NUM_MIN_glDrawArraysIndirect_without_bound (1)

#define FUNID_glDrawElementsIndirect_with_bound ((EXPRESS_GPU_FUN_ID << 32u) + 404)

#define PARA_NUM_MIN_glDrawElementsIndirect_with_bound (1)

#define FUNID_glDrawElementsIndirect_without_bound ((EXPRESS_GPU_FUN_ID << 32u) + 405)

#define PARA_NUM_MIN_glDrawElementsIndirect_without_bound (1)

#define FUNID_glDiscardFramebufferEXT ((EXPRESS_GPU_FUN_ID << 32u) + 406)

#define PARA_NUM_MIN_glDiscardFramebufferEXT (1)

#define FUNID_glTexBuffer ((EXPRESS_GPU_FUN_ID << 32u) + 407)

#define PARA_NUM_MIN_glTexBuffer (1)

#define FUNID_glTexBufferRange ((EXPRESS_GPU_FUN_ID << 32u) + 408)

#define PARA_NUM_MIN_glTexBufferRange (1)

#define FUNID_glColorMaski ((EXPRESS_GPU_FUN_ID << 32u) + 409)

#define PARA_NUM_MIN_glColorMaski (1)

#define FUNID_glBlendFuncSeparatei ((EXPRESS_GPU_FUN_ID << 32u) + 410)

#define PARA_NUM_MIN_glBlendFuncSeparatei (1)

#define FUNID_glBlendEquationSeparatei ((EXPRESS_GPU_FUN_ID << 32u) + 411)

#define PARA_NUM_MIN_glBlendEquationSeparatei (1)

void gl3_decode_invoke(Render_Thread_Context *context, Direct_Express_Call *call);

#endif
