#include "OpenGLFunctions.h"

#include "GLFW/glfw3.h"
#include "common/AQUARIUM_ASSERT.h"

namespace aquarium
{

bool OpenGLFunctions::initialize(BACKENDTYPE backend)
{
    backendType = backend;

    PFNGLGETSTRINGPROC getString;

    if (backendType == BACKENDTYPE::BACKENDTYPEANGLE)
    {
#ifdef _WIN32
        hinstance = LoadLibraryA("libGLESv2.dll");

        if (hinstance == nullptr)
        {
            std::cout << "Failed to load OpenGL[es] library" << std::endl;
            return false;
        }

        getString = reinterpret_cast<PFNGLGETSTRINGPROC>(GetProcAddress(hinstance, "glGetString"));
#endif
    }
    else  // OpenGL
    {
        getString = reinterpret_cast<PFNGLGETSTRINGPROC>(glfwGetProcAddress("glGetString"));
    }

    if (getString == nullptr)
    {
        return false;
    }

    std::string version = reinterpret_cast<const char *>(getString(GL_VERSION));

    if (version.find("OpenGL ES") != std::string::npos)
    {
        uint32_t mMajorVersion = version[10] - '0';
        uint32_t mMinorVersion = version[12] - '0';

        // The minor version shouldn't get to two digits.
        ASSERT(version.size() <= 13 || !isdigit(version[13]));

        return LoadOpenGLESProcs(mMajorVersion, mMinorVersion);
    }
    else
    {
        uint32_t mMajorVersion = version[0] - '0';
        uint32_t mMinorVersion = version[2] - '0';

        // The minor version shouldn't get to two digits.
        ASSERT(version.size() <= 3 || !isdigit(version[3]));

        return LoadDesktopGLProcs(mMajorVersion, mMinorVersion);
    }

    return false;
}

template <typename T>
bool OpenGLFunctions::LoadProc(T *memberProc, const char *name)
{
    if (backendType == BACKENDTYPE::BACKENDTYPEANGLE)
    {
#ifdef _WIN32
        *memberProc = reinterpret_cast<T>(GetProcAddress(hinstance, name));
#endif
    }
    else  // OpenGL
    {
        *memberProc = reinterpret_cast<T>(glfwGetProcAddress(name));
    }

    ASSERT(memberProc != nullptr);

    if (memberProc == nullptr)
    {
        return false;
    }

    return true;
}

bool OpenGLFunctions::LoadOpenGLESProcs(int majorVersion, int minorVersion)
{
    // OpenGL ES 2.0
    if (majorVersion > 2 || (majorVersion == 2 && minorVersion >= 0))
    {
        (LoadProc(&ActiveTexture, "glActiveTexture"));
        (LoadProc(&AttachShader, "glAttachShader"));
        (LoadProc(&BindAttribLocation, "glBindAttribLocation"));
        (LoadProc(&BindBuffer, "glBindBuffer"));
        (LoadProc(&BindFramebuffer, "glBindFramebuffer"));
        (LoadProc(&BindRenderbuffer, "glBindRenderbuffer"));
        (LoadProc(&BindTexture, "glBindTexture"));
        (LoadProc(&BlendColor, "glBlendColor"));
        (LoadProc(&BlendEquation, "glBlendEquation"));
        (LoadProc(&BlendEquationSeparate, "glBlendEquationSeparate"));
        (LoadProc(&BlendFunc, "glBlendFunc"));
        (LoadProc(&BlendFuncSeparate, "glBlendFuncSeparate"));
        (LoadProc(&BufferData, "glBufferData"));
        (LoadProc(&BufferSubData, "glBufferSubData"));
        (LoadProc(&CheckFramebufferStatus, "glCheckFramebufferStatus"));
        (LoadProc(&Clear, "glClear"));
        (LoadProc(&ClearColor, "glClearColor"));
        (LoadProc(&ClearDepthf, "glClearDepthf"));
        (LoadProc(&ClearStencil, "glClearStencil"));
        (LoadProc(&ColorMask, "glColorMask"));
        (LoadProc(&CompileShader, "glCompileShader"));
        (LoadProc(&CompressedTexImage2D, "glCompressedTexImage2D"));
        (LoadProc(&CompressedTexSubImage2D, "glCompressedTexSubImage2D"));
        (LoadProc(&CopyTexImage2D, "glCopyTexImage2D"));
        (LoadProc(&CopyTexSubImage2D, "glCopyTexSubImage2D"));
        (LoadProc(&CreateProgram, "glCreateProgram"));
        (LoadProc(&CreateShader, "glCreateShader"));
        (LoadProc(&CullFace, "glCullFace"));
        (LoadProc(&DeleteBuffers, "glDeleteBuffers"));
        (LoadProc(&DeleteFramebuffers, "glDeleteFramebuffers"));
        (LoadProc(&DeleteProgram, "glDeleteProgram"));
        (LoadProc(&DeleteRenderbuffers, "glDeleteRenderbuffers"));
        (LoadProc(&DeleteShader, "glDeleteShader"));
        (LoadProc(&DeleteTextures, "glDeleteTextures"));
        (LoadProc(&DepthFunc, "glDepthFunc"));
        (LoadProc(&DepthMask, "glDepthMask"));
        (LoadProc(&DepthRangef, "glDepthRangef"));
        (LoadProc(&DetachShader, "glDetachShader"));
        (LoadProc(&Disable, "glDisable"));
        (LoadProc(&DisableVertexAttribArray, "glDisableVertexAttribArray"));
        (LoadProc(&DrawArrays, "glDrawArrays"));
        (LoadProc(&DrawElements, "glDrawElements"));
        (LoadProc(&Enable, "glEnable"));
        (LoadProc(&EnableVertexAttribArray, "glEnableVertexAttribArray"));
        (LoadProc(&Finish, "glFinish"));
        (LoadProc(&Flush, "glFlush"));
        (LoadProc(&FramebufferRenderbuffer, "glFramebufferRenderbuffer"));
        (LoadProc(&FramebufferTexture2D, "glFramebufferTexture2D"));
        (LoadProc(&FrontFace, "glFrontFace"));
        (LoadProc(&GenBuffers, "glGenBuffers"));
        (LoadProc(&GenerateMipmap, "glGenerateMipmap"));
        (LoadProc(&GenFramebuffers, "glGenFramebuffers"));
        (LoadProc(&GenRenderbuffers, "glGenRenderbuffers"));
        (LoadProc(&GenTextures, "glGenTextures"));
        (LoadProc(&GetActiveAttrib, "glGetActiveAttrib"));
        (LoadProc(&GetActiveUniform, "glGetActiveUniform"));
        (LoadProc(&GetAttachedShaders, "glGetAttachedShaders"));
        (LoadProc(&GetAttribLocation, "glGetAttribLocation"));
        (LoadProc(&GetBooleanv, "glGetBooleanv"));
        (LoadProc(&GetBufferParameteriv, "glGetBufferParameteriv"));
        (LoadProc(&GetError, "glGetError"));
        (LoadProc(&GetFloatv, "glGetFloatv"));
        (LoadProc(&GetFramebufferAttachmentParameteriv, "glGetFramebufferAttachmentParameteriv"));
        (LoadProc(&GetIntegerv, "glGetIntegerv"));
        (LoadProc(&GetProgramiv, "glGetProgramiv"));
        (LoadProc(&GetProgramInfoLog, "glGetProgramInfoLog"));
        (LoadProc(&GetRenderbufferParameteriv, "glGetRenderbufferParameteriv"));
        (LoadProc(&GetShaderiv, "glGetShaderiv"));
        (LoadProc(&GetShaderInfoLog, "glGetShaderInfoLog"));
        (LoadProc(&GetShaderPrecisionFormat, "glGetShaderPrecisionFormat"));
        (LoadProc(&GetShaderSource, "glGetShaderSource"));
        (LoadProc(&GetString, "glGetString"));
        (LoadProc(&GetTexParameterfv, "glGetTexParameterfv"));
        (LoadProc(&GetTexParameteriv, "glGetTexParameteriv"));
        (LoadProc(&GetUniformfv, "glGetUniformfv"));
        (LoadProc(&GetUniformiv, "glGetUniformiv"));
        (LoadProc(&GetUniformLocation, "glGetUniformLocation"));
        (LoadProc(&GetVertexAttribfv, "glGetVertexAttribfv"));
        (LoadProc(&GetVertexAttribiv, "glGetVertexAttribiv"));
        (LoadProc(&GetVertexAttribPointerv, "glGetVertexAttribPointerv"));
        (LoadProc(&Hint, "glHint"));
        (LoadProc(&IsBuffer, "glIsBuffer"));
        (LoadProc(&IsEnabled, "glIsEnabled"));
        (LoadProc(&IsFramebuffer, "glIsFramebuffer"));
        (LoadProc(&IsProgram, "glIsProgram"));
        (LoadProc(&IsRenderbuffer, "glIsRenderbuffer"));
        (LoadProc(&IsShader, "glIsShader"));
        (LoadProc(&IsTexture, "glIsTexture"));
        (LoadProc(&LineWidth, "glLineWidth"));
        (LoadProc(&LinkProgram, "glLinkProgram"));
        (LoadProc(&PixelStorei, "glPixelStorei"));
        (LoadProc(&PolygonOffset, "glPolygonOffset"));
        (LoadProc(&ReadPixels, "glReadPixels"));
        (LoadProc(&ReleaseShaderCompiler, "glReleaseShaderCompiler"));
        (LoadProc(&RenderbufferStorage, "glRenderbufferStorage"));
        (LoadProc(&SampleCoverage, "glSampleCoverage"));
        (LoadProc(&Scissor, "glScissor"));
        (LoadProc(&ShaderBinary, "glShaderBinary"));
        (LoadProc(&ShaderSource, "glShaderSource"));
        (LoadProc(&StencilFunc, "glStencilFunc"));
        (LoadProc(&StencilFuncSeparate, "glStencilFuncSeparate"));
        (LoadProc(&StencilMask, "glStencilMask"));
        (LoadProc(&StencilMaskSeparate, "glStencilMaskSeparate"));
        (LoadProc(&StencilOp, "glStencilOp"));
        (LoadProc(&StencilOpSeparate, "glStencilOpSeparate"));
        (LoadProc(&TexImage2D, "glTexImage2D"));
        (LoadProc(&TexParameterf, "glTexParameterf"));
        (LoadProc(&TexParameterfv, "glTexParameterfv"));
        (LoadProc(&TexParameteri, "glTexParameteri"));
        (LoadProc(&TexParameteriv, "glTexParameteriv"));
        (LoadProc(&TexSubImage2D, "glTexSubImage2D"));
        (LoadProc(&Uniform1f, "glUniform1f"));
        (LoadProc(&Uniform1fv, "glUniform1fv"));
        (LoadProc(&Uniform1i, "glUniform1i"));
        (LoadProc(&Uniform1iv, "glUniform1iv"));
        (LoadProc(&Uniform2f, "glUniform2f"));
        (LoadProc(&Uniform2fv, "glUniform2fv"));
        (LoadProc(&Uniform2i, "glUniform2i"));
        (LoadProc(&Uniform2iv, "glUniform2iv"));
        (LoadProc(&Uniform3f, "glUniform3f"));
        (LoadProc(&Uniform3fv, "glUniform3fv"));
        (LoadProc(&Uniform3i, "glUniform3i"));
        (LoadProc(&Uniform3iv, "glUniform3iv"));
        (LoadProc(&Uniform4f, "glUniform4f"));
        (LoadProc(&Uniform4fv, "glUniform4fv"));
        (LoadProc(&Uniform4i, "glUniform4i"));
        (LoadProc(&Uniform4iv, "glUniform4iv"));
        (LoadProc(&UniformMatrix2fv, "glUniformMatrix2fv"));
        (LoadProc(&UniformMatrix3fv, "glUniformMatrix3fv"));
        (LoadProc(&UniformMatrix4fv, "glUniformMatrix4fv"));
        (LoadProc(&UseProgram, "glUseProgram"));
        (LoadProc(&ValidateProgram, "glValidateProgram"));
        (LoadProc(&VertexAttrib1f, "glVertexAttrib1f"));
        (LoadProc(&VertexAttrib1fv, "glVertexAttrib1fv"));
        (LoadProc(&VertexAttrib2f, "glVertexAttrib2f"));
        (LoadProc(&VertexAttrib2fv, "glVertexAttrib2fv"));
        (LoadProc(&VertexAttrib3f, "glVertexAttrib3f"));
        (LoadProc(&VertexAttrib3fv, "glVertexAttrib3fv"));
        (LoadProc(&VertexAttrib4f, "glVertexAttrib4f"));
        (LoadProc(&VertexAttrib4fv, "glVertexAttrib4fv"));
        (LoadProc(&VertexAttribPointer, "glVertexAttribPointer"));
        (LoadProc(&Viewport, "glViewport"));
    }

    // OpenGL ES 3.0
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 0))
    {
        (LoadProc(&ReadBuffer, "glReadBuffer"));
        (LoadProc(&DrawRangeElements, "glDrawRangeElements"));
        (LoadProc(&TexImage3D, "glTexImage3D"));
        (LoadProc(&TexSubImage3D, "glTexSubImage3D"));
        (LoadProc(&CopyTexSubImage3D, "glCopyTexSubImage3D"));
        (LoadProc(&CompressedTexImage3D, "glCompressedTexImage3D"));
        (LoadProc(&CompressedTexSubImage3D, "glCompressedTexSubImage3D"));
        (LoadProc(&GenQueries, "glGenQueries"));
        (LoadProc(&DeleteQueries, "glDeleteQueries"));
        (LoadProc(&IsQuery, "glIsQuery"));
        (LoadProc(&BeginQuery, "glBeginQuery"));
        (LoadProc(&EndQuery, "glEndQuery"));
        (LoadProc(&GetQueryiv, "glGetQueryiv"));
        (LoadProc(&GetQueryObjectuiv, "glGetQueryObjectuiv"));
        (LoadProc(&UnmapBuffer, "glUnmapBuffer"));
        (LoadProc(&GetBufferPointerv, "glGetBufferPointerv"));
        (LoadProc(&DrawBuffers, "glDrawBuffers"));
        (LoadProc(&UniformMatrix2x3fv, "glUniformMatrix2x3fv"));
        (LoadProc(&UniformMatrix3x2fv, "glUniformMatrix3x2fv"));
        (LoadProc(&UniformMatrix2x4fv, "glUniformMatrix2x4fv"));
        (LoadProc(&UniformMatrix4x2fv, "glUniformMatrix4x2fv"));
        (LoadProc(&UniformMatrix3x4fv, "glUniformMatrix3x4fv"));
        (LoadProc(&UniformMatrix4x3fv, "glUniformMatrix4x3fv"));
        (LoadProc(&BlitFramebuffer, "glBlitFramebuffer"));
        (LoadProc(&RenderbufferStorageMultisample, "glRenderbufferStorageMultisample"));
        (LoadProc(&FramebufferTextureLayer, "glFramebufferTextureLayer"));
        (LoadProc(&MapBufferRange, "glMapBufferRange"));
        (LoadProc(&FlushMappedBufferRange, "glFlushMappedBufferRange"));
        (LoadProc(&BindVertexArray, "glBindVertexArray"));
        (LoadProc(&DeleteVertexArrays, "glDeleteVertexArrays"));
        (LoadProc(&GenVertexArrays, "glGenVertexArrays"));
        (LoadProc(&IsVertexArray, "glIsVertexArray"));
        (LoadProc(&GetIntegeri_v, "glGetIntegeri_v"));
        (LoadProc(&BeginTransformFeedback, "glBeginTransformFeedback"));
        (LoadProc(&EndTransformFeedback, "glEndTransformFeedback"));
        (LoadProc(&BindBufferRange, "glBindBufferRange"));
        (LoadProc(&BindBufferBase, "glBindBufferBase"));
        (LoadProc(&TransformFeedbackVaryings, "glTransformFeedbackVaryings"));
        (LoadProc(&GetTransformFeedbackVarying, "glGetTransformFeedbackVarying"));
        (LoadProc(&VertexAttribIPointer, "glVertexAttribIPointer"));
        (LoadProc(&GetVertexAttribIiv, "glGetVertexAttribIiv"));
        (LoadProc(&GetVertexAttribIuiv, "glGetVertexAttribIuiv"));
        (LoadProc(&VertexAttribI4i, "glVertexAttribI4i"));
        (LoadProc(&VertexAttribI4ui, "glVertexAttribI4ui"));
        (LoadProc(&VertexAttribI4iv, "glVertexAttribI4iv"));
        (LoadProc(&VertexAttribI4uiv, "glVertexAttribI4uiv"));
        (LoadProc(&GetUniformuiv, "glGetUniformuiv"));
        (LoadProc(&GetFragDataLocation, "glGetFragDataLocation"));
        (LoadProc(&Uniform1ui, "glUniform1ui"));
        (LoadProc(&Uniform2ui, "glUniform2ui"));
        (LoadProc(&Uniform3ui, "glUniform3ui"));
        (LoadProc(&Uniform4ui, "glUniform4ui"));
        (LoadProc(&Uniform1uiv, "glUniform1uiv"));
        (LoadProc(&Uniform2uiv, "glUniform2uiv"));
        (LoadProc(&Uniform3uiv, "glUniform3uiv"));
        (LoadProc(&Uniform4uiv, "glUniform4uiv"));
        (LoadProc(&ClearBufferiv, "glClearBufferiv"));
        (LoadProc(&ClearBufferuiv, "glClearBufferuiv"));
        (LoadProc(&ClearBufferfv, "glClearBufferfv"));
        (LoadProc(&ClearBufferfi, "glClearBufferfi"));
        (LoadProc(&GetStringi, "glGetStringi"));
        (LoadProc(&CopyBufferSubData, "glCopyBufferSubData"));
        (LoadProc(&GetUniformIndices, "glGetUniformIndices"));
        (LoadProc(&GetActiveUniformsiv, "glGetActiveUniformsiv"));
        (LoadProc(&GetUniformBlockIndex, "glGetUniformBlockIndex"));
        (LoadProc(&GetActiveUniformBlockiv, "glGetActiveUniformBlockiv"));
        (LoadProc(&GetActiveUniformBlockName, "glGetActiveUniformBlockName"));
        (LoadProc(&UniformBlockBinding, "glUniformBlockBinding"));
        (LoadProc(&DrawArraysInstanced, "glDrawArraysInstanced"));
        (LoadProc(&DrawElementsInstanced, "glDrawElementsInstanced"));
        (LoadProc(&FenceSync, "glFenceSync"));
        (LoadProc(&IsSync, "glIsSync"));
        (LoadProc(&DeleteSync, "glDeleteSync"));
        (LoadProc(&ClientWaitSync, "glClientWaitSync"));
        (LoadProc(&WaitSync, "glWaitSync"));
        (LoadProc(&GetInteger64v, "glGetInteger64v"));
        (LoadProc(&GetSynciv, "glGetSynciv"));
        (LoadProc(&GetInteger64i_v, "glGetInteger64i_v"));
        (LoadProc(&GetBufferParameteri64v, "glGetBufferParameteri64v"));
        (LoadProc(&GenSamplers, "glGenSamplers"));
        (LoadProc(&DeleteSamplers, "glDeleteSamplers"));
        (LoadProc(&IsSampler, "glIsSampler"));
        (LoadProc(&BindSampler, "glBindSampler"));
        (LoadProc(&SamplerParameteri, "glSamplerParameteri"));
        (LoadProc(&SamplerParameteriv, "glSamplerParameteriv"));
        (LoadProc(&SamplerParameterf, "glSamplerParameterf"));
        (LoadProc(&SamplerParameterfv, "glSamplerParameterfv"));
        (LoadProc(&GetSamplerParameteriv, "glGetSamplerParameteriv"));
        (LoadProc(&GetSamplerParameterfv, "glGetSamplerParameterfv"));
        (LoadProc(&VertexAttribDivisor, "glVertexAttribDivisor"));
        (LoadProc(&BindTransformFeedback, "glBindTransformFeedback"));
        (LoadProc(&DeleteTransformFeedbacks, "glDeleteTransformFeedbacks"));
        (LoadProc(&GenTransformFeedbacks, "glGenTransformFeedbacks"));
        (LoadProc(&IsTransformFeedback, "glIsTransformFeedback"));
        (LoadProc(&PauseTransformFeedback, "glPauseTransformFeedback"));
        (LoadProc(&ResumeTransformFeedback, "glResumeTransformFeedback"));
        (LoadProc(&GetProgramBinary, "glGetProgramBinary"));
        (LoadProc(&ProgramBinary, "glProgramBinary"));
        (LoadProc(&ProgramParameteri, "glProgramParameteri"));
        (LoadProc(&InvalidateFramebuffer, "glInvalidateFramebuffer"));
        (LoadProc(&InvalidateSubFramebuffer, "glInvalidateSubFramebuffer"));
        (LoadProc(&TexStorage2D, "glTexStorage2D"));
        (LoadProc(&TexStorage3D, "glTexStorage3D"));
        (LoadProc(&GetInternalformativ, "glGetInternalformativ"));
    }

    // OpenGL ES 3.1
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 1))
    {
        (LoadProc(&DispatchCompute, "glDispatchCompute"));
        (LoadProc(&DispatchComputeIndirect, "glDispatchComputeIndirect"));
        (LoadProc(&DrawArraysIndirect, "glDrawArraysIndirect"));
        (LoadProc(&DrawElementsIndirect, "glDrawElementsIndirect"));
        (LoadProc(&FramebufferParameteri, "glFramebufferParameteri"));
        (LoadProc(&GetFramebufferParameteriv, "glGetFramebufferParameteriv"));
        (LoadProc(&GetProgramInterfaceiv, "glGetProgramInterfaceiv"));
        (LoadProc(&GetProgramResourceIndex, "glGetProgramResourceIndex"));
        (LoadProc(&GetProgramResourceName, "glGetProgramResourceName"));
        (LoadProc(&GetProgramResourceiv, "glGetProgramResourceiv"));
        (LoadProc(&GetProgramResourceLocation, "glGetProgramResourceLocation"));
        (LoadProc(&UseProgramStages, "glUseProgramStages"));
        (LoadProc(&ActiveShaderProgram, "glActiveShaderProgram"));
        (LoadProc(&CreateShaderProgramv, "glCreateShaderProgramv"));
        (LoadProc(&BindProgramPipeline, "glBindProgramPipeline"));
        (LoadProc(&DeleteProgramPipelines, "glDeleteProgramPipelines"));
        (LoadProc(&GenProgramPipelines, "glGenProgramPipelines"));
        (LoadProc(&IsProgramPipeline, "glIsProgramPipeline"));
        (LoadProc(&GetProgramPipelineiv, "glGetProgramPipelineiv"));
        (LoadProc(&ProgramUniform1i, "glProgramUniform1i"));
        (LoadProc(&ProgramUniform2i, "glProgramUniform2i"));
        (LoadProc(&ProgramUniform3i, "glProgramUniform3i"));
        (LoadProc(&ProgramUniform4i, "glProgramUniform4i"));
        (LoadProc(&ProgramUniform1ui, "glProgramUniform1ui"));
        (LoadProc(&ProgramUniform2ui, "glProgramUniform2ui"));
        (LoadProc(&ProgramUniform3ui, "glProgramUniform3ui"));
        (LoadProc(&ProgramUniform4ui, "glProgramUniform4ui"));
        (LoadProc(&ProgramUniform1f, "glProgramUniform1f"));
        (LoadProc(&ProgramUniform2f, "glProgramUniform2f"));
        (LoadProc(&ProgramUniform3f, "glProgramUniform3f"));
        (LoadProc(&ProgramUniform4f, "glProgramUniform4f"));
        (LoadProc(&ProgramUniform1iv, "glProgramUniform1iv"));
        (LoadProc(&ProgramUniform2iv, "glProgramUniform2iv"));
        (LoadProc(&ProgramUniform3iv, "glProgramUniform3iv"));
        (LoadProc(&ProgramUniform4iv, "glProgramUniform4iv"));
        (LoadProc(&ProgramUniform1uiv, "glProgramUniform1uiv"));
        (LoadProc(&ProgramUniform2uiv, "glProgramUniform2uiv"));
        (LoadProc(&ProgramUniform3uiv, "glProgramUniform3uiv"));
        (LoadProc(&ProgramUniform4uiv, "glProgramUniform4uiv"));
        (LoadProc(&ProgramUniform1fv, "glProgramUniform1fv"));
        (LoadProc(&ProgramUniform2fv, "glProgramUniform2fv"));
        (LoadProc(&ProgramUniform3fv, "glProgramUniform3fv"));
        (LoadProc(&ProgramUniform4fv, "glProgramUniform4fv"));
        (LoadProc(&ProgramUniformMatrix2fv, "glProgramUniformMatrix2fv"));
        (LoadProc(&ProgramUniformMatrix3fv, "glProgramUniformMatrix3fv"));
        (LoadProc(&ProgramUniformMatrix4fv, "glProgramUniformMatrix4fv"));
        (LoadProc(&ProgramUniformMatrix2x3fv, "glProgramUniformMatrix2x3fv"));
        (LoadProc(&ProgramUniformMatrix3x2fv, "glProgramUniformMatrix3x2fv"));
        (LoadProc(&ProgramUniformMatrix2x4fv, "glProgramUniformMatrix2x4fv"));
        (LoadProc(&ProgramUniformMatrix4x2fv, "glProgramUniformMatrix4x2fv"));
        (LoadProc(&ProgramUniformMatrix3x4fv, "glProgramUniformMatrix3x4fv"));
        (LoadProc(&ProgramUniformMatrix4x3fv, "glProgramUniformMatrix4x3fv"));
        (LoadProc(&ValidateProgramPipeline, "glValidateProgramPipeline"));
        (LoadProc(&GetProgramPipelineInfoLog, "glGetProgramPipelineInfoLog"));
        (LoadProc(&BindImageTexture, "glBindImageTexture"));
        (LoadProc(&GetBooleani_v, "glGetBooleani_v"));
        (LoadProc(&MemoryBarrier, "glMemoryBarrier"));
        (LoadProc(&MemoryBarrierByRegion, "glMemoryBarrierByRegion"));
        (LoadProc(&TexStorage2DMultisample, "glTexStorage2DMultisample"));
        (LoadProc(&GetMultisamplefv, "glGetMultisamplefv"));
        (LoadProc(&SampleMaski, "glSampleMaski"));
        (LoadProc(&GetTexLevelParameteriv, "glGetTexLevelParameteriv"));
        (LoadProc(&GetTexLevelParameterfv, "glGetTexLevelParameterfv"));
        (LoadProc(&BindVertexBuffer, "glBindVertexBuffer"));
        (LoadProc(&VertexAttribFormat, "glVertexAttribFormat"));
        (LoadProc(&VertexAttribIFormat, "glVertexAttribIFormat"));
        (LoadProc(&VertexAttribBinding, "glVertexAttribBinding"));
        (LoadProc(&VertexBindingDivisor, "glVertexBindingDivisor"));
    }

    // OpenGL ES 3.2
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 2))
    {
        (LoadProc(&BlendBarrier, "glBlendBarrier"));
        (LoadProc(&CopyImageSubData, "glCopyImageSubData"));
        (LoadProc(&DebugMessageControl, "glDebugMessageControl"));
        (LoadProc(&DebugMessageInsert, "glDebugMessageInsert"));
        (LoadProc(&DebugMessageCallback, "glDebugMessageCallback"));
        (LoadProc(&GetDebugMessageLog, "glGetDebugMessageLog"));
        (LoadProc(&PushDebugGroup, "glPushDebugGroup"));
        (LoadProc(&PopDebugGroup, "glPopDebugGroup"));
        (LoadProc(&ObjectLabel, "glObjectLabel"));
        (LoadProc(&GetObjectLabel, "glGetObjectLabel"));
        (LoadProc(&ObjectPtrLabel, "glObjectPtrLabel"));
        (LoadProc(&GetObjectPtrLabel, "glGetObjectPtrLabel"));
        (LoadProc(&GetPointerv, "glGetPointerv"));
        (LoadProc(&Enablei, "glEnablei"));
        (LoadProc(&Disablei, "glDisablei"));
        (LoadProc(&BlendEquationi, "glBlendEquationi"));
        (LoadProc(&BlendEquationSeparatei, "glBlendEquationSeparatei"));
        (LoadProc(&BlendFunci, "glBlendFunci"));
        (LoadProc(&BlendFuncSeparatei, "glBlendFuncSeparatei"));
        (LoadProc(&ColorMaski, "glColorMaski"));
        (LoadProc(&IsEnabledi, "glIsEnabledi"));
        (LoadProc(&DrawElementsBaseVertex, "glDrawElementsBaseVertex"));
        (LoadProc(&DrawRangeElementsBaseVertex, "glDrawRangeElementsBaseVertex"));
        (LoadProc(&DrawElementsInstancedBaseVertex, "glDrawElementsInstancedBaseVertex"));
        (LoadProc(&FramebufferTexture, "glFramebufferTexture"));
        (LoadProc(&PrimitiveBoundingBox, "glPrimitiveBoundingBox"));
        (LoadProc(&GetGraphicsResetStatus, "glGetGraphicsResetStatus"));
        (LoadProc(&ReadnPixels, "glReadnPixels"));
        (LoadProc(&GetnUniformfv, "glGetnUniformfv"));
        (LoadProc(&GetnUniformiv, "glGetnUniformiv"));
        (LoadProc(&GetnUniformuiv, "glGetnUniformuiv"));
        (LoadProc(&MinSampleShading, "glMinSampleShading"));
        (LoadProc(&PatchParameteri, "glPatchParameteri"));
        (LoadProc(&TexParameterIiv, "glTexParameterIiv"));
        (LoadProc(&TexParameterIuiv, "glTexParameterIuiv"));
        (LoadProc(&GetTexParameterIiv, "glGetTexParameterIiv"));
        (LoadProc(&GetTexParameterIuiv, "glGetTexParameterIuiv"));
        (LoadProc(&SamplerParameterIiv, "glSamplerParameterIiv"));
        (LoadProc(&SamplerParameterIuiv, "glSamplerParameterIuiv"));
        (LoadProc(&GetSamplerParameterIiv, "glGetSamplerParameterIiv"));
        (LoadProc(&GetSamplerParameterIuiv, "glGetSamplerParameterIuiv"));
        (LoadProc(&TexBuffer, "glTexBuffer"));
        (LoadProc(&TexBufferRange, "glTexBufferRange"));
        (LoadProc(&TexStorage3DMultisample, "glTexStorage3DMultisample"));
    }

    return true;
}

bool OpenGLFunctions::LoadDesktopGLProcs(int majorVersion, int minorVersion)
{
    // Desktop OpenGL 1.0
    if (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 0))
    {
        (LoadProc(&CullFace, "glCullFace"));
        (LoadProc(&FrontFace, "glFrontFace"));
        (LoadProc(&Hint, "glHint"));
        (LoadProc(&LineWidth, "glLineWidth"));
        (LoadProc(&PointSize, "glPointSize"));
        (LoadProc(&PolygonMode, "glPolygonMode"));
        (LoadProc(&Scissor, "glScissor"));
        (LoadProc(&TexParameterf, "glTexParameterf"));
        (LoadProc(&TexParameterfv, "glTexParameterfv"));
        (LoadProc(&TexParameteri, "glTexParameteri"));
        (LoadProc(&TexParameteriv, "glTexParameteriv"));
        (LoadProc(&TexImage1D, "glTexImage1D"));
        (LoadProc(&TexImage2D, "glTexImage2D"));
        (LoadProc(&DrawBuffer, "glDrawBuffer"));
        (LoadProc(&Clear, "glClear"));
        (LoadProc(&ClearColor, "glClearColor"));
        (LoadProc(&ClearStencil, "glClearStencil"));
        (LoadProc(&ClearDepth, "glClearDepth"));
        (LoadProc(&StencilMask, "glStencilMask"));
        (LoadProc(&ColorMask, "glColorMask"));
        (LoadProc(&DepthMask, "glDepthMask"));
        (LoadProc(&Disable, "glDisable"));
        (LoadProc(&Enable, "glEnable"));
        (LoadProc(&Finish, "glFinish"));
        (LoadProc(&Flush, "glFlush"));
        (LoadProc(&BlendFunc, "glBlendFunc"));
        (LoadProc(&LogicOp, "glLogicOp"));
        (LoadProc(&StencilFunc, "glStencilFunc"));
        (LoadProc(&StencilOp, "glStencilOp"));
        (LoadProc(&DepthFunc, "glDepthFunc"));
        (LoadProc(&PixelStoref, "glPixelStoref"));
        (LoadProc(&PixelStorei, "glPixelStorei"));
        (LoadProc(&ReadBuffer, "glReadBuffer"));
        (LoadProc(&ReadPixels, "glReadPixels"));
        (LoadProc(&GetBooleanv, "glGetBooleanv"));
        (LoadProc(&GetDoublev, "glGetDoublev"));
        (LoadProc(&GetError, "glGetError"));
        (LoadProc(&GetFloatv, "glGetFloatv"));
        (LoadProc(&GetIntegerv, "glGetIntegerv"));
        (LoadProc(&GetString, "glGetString"));
        (LoadProc(&GetTexImage, "glGetTexImage"));
        (LoadProc(&GetTexParameterfv, "glGetTexParameterfv"));
        (LoadProc(&GetTexParameteriv, "glGetTexParameteriv"));
        (LoadProc(&GetTexLevelParameterfv, "glGetTexLevelParameterfv"));
        (LoadProc(&GetTexLevelParameteriv, "glGetTexLevelParameteriv"));
        (LoadProc(&IsEnabled, "glIsEnabled"));
        (LoadProc(&DepthRange, "glDepthRange"));
        (LoadProc(&Viewport, "glViewport"));
    }

    // Desktop OpenGL 1.1
    if (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 1))
    {
        (LoadProc(&DrawArrays, "glDrawArrays"));
        (LoadProc(&DrawElements, "glDrawElements"));
        (LoadProc(&PolygonOffset, "glPolygonOffset"));
        (LoadProc(&CopyTexImage1D, "glCopyTexImage1D"));
        (LoadProc(&CopyTexImage2D, "glCopyTexImage2D"));
        (LoadProc(&CopyTexSubImage1D, "glCopyTexSubImage1D"));
        (LoadProc(&CopyTexSubImage2D, "glCopyTexSubImage2D"));
        (LoadProc(&TexSubImage1D, "glTexSubImage1D"));
        (LoadProc(&TexSubImage2D, "glTexSubImage2D"));
        (LoadProc(&BindTexture, "glBindTexture"));
        (LoadProc(&DeleteTextures, "glDeleteTextures"));
        (LoadProc(&GenTextures, "glGenTextures"));
        (LoadProc(&IsTexture, "glIsTexture"));
    }

    // Desktop OpenGL 1.2
    if (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 2))
    {
        (LoadProc(&DrawRangeElements, "glDrawRangeElements"));
        (LoadProc(&TexImage3D, "glTexImage3D"));
        (LoadProc(&TexSubImage3D, "glTexSubImage3D"));
        (LoadProc(&CopyTexSubImage3D, "glCopyTexSubImage3D"));
    }

    // Desktop OpenGL 1.3
    if (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 3))
    {
        (LoadProc(&ActiveTexture, "glActiveTexture"));
        (LoadProc(&SampleCoverage, "glSampleCoverage"));
        (LoadProc(&CompressedTexImage3D, "glCompressedTexImage3D"));
        (LoadProc(&CompressedTexImage2D, "glCompressedTexImage2D"));
        (LoadProc(&CompressedTexImage1D, "glCompressedTexImage1D"));
        (LoadProc(&CompressedTexSubImage3D, "glCompressedTexSubImage3D"));
        (LoadProc(&CompressedTexSubImage2D, "glCompressedTexSubImage2D"));
        (LoadProc(&CompressedTexSubImage1D, "glCompressedTexSubImage1D"));
        (LoadProc(&GetCompressedTexImage, "glGetCompressedTexImage"));
    }

    // Desktop OpenGL 1.4
    if (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 4))
    {
        (LoadProc(&BlendFuncSeparate, "glBlendFuncSeparate"));
        (LoadProc(&MultiDrawArrays, "glMultiDrawArrays"));
        (LoadProc(&MultiDrawElements, "glMultiDrawElements"));
        (LoadProc(&PointParameterf, "glPointParameterf"));
        (LoadProc(&PointParameterfv, "glPointParameterfv"));
        (LoadProc(&PointParameteri, "glPointParameteri"));
        (LoadProc(&PointParameteriv, "glPointParameteriv"));
        (LoadProc(&BlendColor, "glBlendColor"));
        (LoadProc(&BlendEquation, "glBlendEquation"));
    }

    // Desktop OpenGL 1.5
    if (majorVersion > 1 || (majorVersion == 1 && minorVersion >= 5))
    {
        (LoadProc(&GenQueries, "glGenQueries"));
        (LoadProc(&DeleteQueries, "glDeleteQueries"));
        (LoadProc(&IsQuery, "glIsQuery"));
        (LoadProc(&BeginQuery, "glBeginQuery"));
        (LoadProc(&EndQuery, "glEndQuery"));
        (LoadProc(&GetQueryiv, "glGetQueryiv"));
        (LoadProc(&GetQueryObjectiv, "glGetQueryObjectiv"));
        (LoadProc(&GetQueryObjectuiv, "glGetQueryObjectuiv"));
        (LoadProc(&BindBuffer, "glBindBuffer"));
        (LoadProc(&DeleteBuffers, "glDeleteBuffers"));
        (LoadProc(&GenBuffers, "glGenBuffers"));
        (LoadProc(&IsBuffer, "glIsBuffer"));
        (LoadProc(&BufferData, "glBufferData"));
        (LoadProc(&BufferSubData, "glBufferSubData"));
        (LoadProc(&GetBufferSubData, "glGetBufferSubData"));
        (LoadProc(&MapBuffer, "glMapBuffer"));
        (LoadProc(&UnmapBuffer, "glUnmapBuffer"));
        (LoadProc(&GetBufferParameteriv, "glGetBufferParameteriv"));
        (LoadProc(&GetBufferPointerv, "glGetBufferPointerv"));
    }

    // Desktop OpenGL 2.0
    if (majorVersion > 2 || (majorVersion == 2 && minorVersion >= 0))
    {
        (LoadProc(&BlendEquationSeparate, "glBlendEquationSeparate"));
        (LoadProc(&DrawBuffers, "glDrawBuffers"));
        (LoadProc(&StencilOpSeparate, "glStencilOpSeparate"));
        (LoadProc(&StencilFuncSeparate, "glStencilFuncSeparate"));
        (LoadProc(&StencilMaskSeparate, "glStencilMaskSeparate"));
        (LoadProc(&AttachShader, "glAttachShader"));
        (LoadProc(&BindAttribLocation, "glBindAttribLocation"));
        (LoadProc(&CompileShader, "glCompileShader"));
        (LoadProc(&CreateProgram, "glCreateProgram"));
        (LoadProc(&CreateShader, "glCreateShader"));
        (LoadProc(&DeleteProgram, "glDeleteProgram"));
        (LoadProc(&DeleteShader, "glDeleteShader"));
        (LoadProc(&DetachShader, "glDetachShader"));
        (LoadProc(&DisableVertexAttribArray, "glDisableVertexAttribArray"));
        (LoadProc(&EnableVertexAttribArray, "glEnableVertexAttribArray"));
        (LoadProc(&GetActiveAttrib, "glGetActiveAttrib"));
        (LoadProc(&GetActiveUniform, "glGetActiveUniform"));
        (LoadProc(&GetAttachedShaders, "glGetAttachedShaders"));
        (LoadProc(&GetAttribLocation, "glGetAttribLocation"));
        (LoadProc(&GetProgramiv, "glGetProgramiv"));
        (LoadProc(&GetProgramInfoLog, "glGetProgramInfoLog"));
        (LoadProc(&GetShaderiv, "glGetShaderiv"));
        (LoadProc(&GetShaderInfoLog, "glGetShaderInfoLog"));
        (LoadProc(&GetShaderSource, "glGetShaderSource"));
        (LoadProc(&GetUniformLocation, "glGetUniformLocation"));
        (LoadProc(&GetUniformfv, "glGetUniformfv"));
        (LoadProc(&GetUniformiv, "glGetUniformiv"));
        (LoadProc(&GetVertexAttribdv, "glGetVertexAttribdv"));
        (LoadProc(&GetVertexAttribfv, "glGetVertexAttribfv"));
        (LoadProc(&GetVertexAttribiv, "glGetVertexAttribiv"));
        (LoadProc(&GetVertexAttribPointerv, "glGetVertexAttribPointerv"));
        (LoadProc(&IsProgram, "glIsProgram"));
        (LoadProc(&IsShader, "glIsShader"));
        (LoadProc(&LinkProgram, "glLinkProgram"));
        (LoadProc(&ShaderSource, "glShaderSource"));
        (LoadProc(&UseProgram, "glUseProgram"));
        (LoadProc(&Uniform1f, "glUniform1f"));
        (LoadProc(&Uniform2f, "glUniform2f"));
        (LoadProc(&Uniform3f, "glUniform3f"));
        (LoadProc(&Uniform4f, "glUniform4f"));
        (LoadProc(&Uniform1i, "glUniform1i"));
        (LoadProc(&Uniform2i, "glUniform2i"));
        (LoadProc(&Uniform3i, "glUniform3i"));
        (LoadProc(&Uniform4i, "glUniform4i"));
        (LoadProc(&Uniform1fv, "glUniform1fv"));
        (LoadProc(&Uniform2fv, "glUniform2fv"));
        (LoadProc(&Uniform3fv, "glUniform3fv"));
        (LoadProc(&Uniform4fv, "glUniform4fv"));
        (LoadProc(&Uniform1iv, "glUniform1iv"));
        (LoadProc(&Uniform2iv, "glUniform2iv"));
        (LoadProc(&Uniform3iv, "glUniform3iv"));
        (LoadProc(&Uniform4iv, "glUniform4iv"));
        (LoadProc(&UniformMatrix2fv, "glUniformMatrix2fv"));
        (LoadProc(&UniformMatrix3fv, "glUniformMatrix3fv"));
        (LoadProc(&UniformMatrix4fv, "glUniformMatrix4fv"));
        (LoadProc(&ValidateProgram, "glValidateProgram"));
        (LoadProc(&VertexAttrib1d, "glVertexAttrib1d"));
        (LoadProc(&VertexAttrib1dv, "glVertexAttrib1dv"));
        (LoadProc(&VertexAttrib1f, "glVertexAttrib1f"));
        (LoadProc(&VertexAttrib1fv, "glVertexAttrib1fv"));
        (LoadProc(&VertexAttrib1s, "glVertexAttrib1s"));
        (LoadProc(&VertexAttrib1sv, "glVertexAttrib1sv"));
        (LoadProc(&VertexAttrib2d, "glVertexAttrib2d"));
        (LoadProc(&VertexAttrib2dv, "glVertexAttrib2dv"));
        (LoadProc(&VertexAttrib2f, "glVertexAttrib2f"));
        (LoadProc(&VertexAttrib2fv, "glVertexAttrib2fv"));
        (LoadProc(&VertexAttrib2s, "glVertexAttrib2s"));
        (LoadProc(&VertexAttrib2sv, "glVertexAttrib2sv"));
        (LoadProc(&VertexAttrib3d, "glVertexAttrib3d"));
        (LoadProc(&VertexAttrib3dv, "glVertexAttrib3dv"));
        (LoadProc(&VertexAttrib3f, "glVertexAttrib3f"));
        (LoadProc(&VertexAttrib3fv, "glVertexAttrib3fv"));
        (LoadProc(&VertexAttrib3s, "glVertexAttrib3s"));
        (LoadProc(&VertexAttrib3sv, "glVertexAttrib3sv"));
        (LoadProc(&VertexAttrib4Nbv, "glVertexAttrib4Nbv"));
        (LoadProc(&VertexAttrib4Niv, "glVertexAttrib4Niv"));
        (LoadProc(&VertexAttrib4Nsv, "glVertexAttrib4Nsv"));
        (LoadProc(&VertexAttrib4Nub, "glVertexAttrib4Nub"));
        (LoadProc(&VertexAttrib4Nubv, "glVertexAttrib4Nubv"));
        (LoadProc(&VertexAttrib4Nuiv, "glVertexAttrib4Nuiv"));
        (LoadProc(&VertexAttrib4Nusv, "glVertexAttrib4Nusv"));
        (LoadProc(&VertexAttrib4bv, "glVertexAttrib4bv"));
        (LoadProc(&VertexAttrib4d, "glVertexAttrib4d"));
        (LoadProc(&VertexAttrib4dv, "glVertexAttrib4dv"));
        (LoadProc(&VertexAttrib4f, "glVertexAttrib4f"));
        (LoadProc(&VertexAttrib4fv, "glVertexAttrib4fv"));
        (LoadProc(&VertexAttrib4iv, "glVertexAttrib4iv"));
        (LoadProc(&VertexAttrib4s, "glVertexAttrib4s"));
        (LoadProc(&VertexAttrib4sv, "glVertexAttrib4sv"));
        (LoadProc(&VertexAttrib4ubv, "glVertexAttrib4ubv"));
        (LoadProc(&VertexAttrib4uiv, "glVertexAttrib4uiv"));
        (LoadProc(&VertexAttrib4usv, "glVertexAttrib4usv"));
        (LoadProc(&VertexAttribPointer, "glVertexAttribPointer"));
    }

    // Desktop OpenGL 2.1
    if (majorVersion > 2 || (majorVersion == 2 && minorVersion >= 1))
    {
        (LoadProc(&UniformMatrix2x3fv, "glUniformMatrix2x3fv"));
        (LoadProc(&UniformMatrix3x2fv, "glUniformMatrix3x2fv"));
        (LoadProc(&UniformMatrix2x4fv, "glUniformMatrix2x4fv"));
        (LoadProc(&UniformMatrix4x2fv, "glUniformMatrix4x2fv"));
        (LoadProc(&UniformMatrix3x4fv, "glUniformMatrix3x4fv"));
        (LoadProc(&UniformMatrix4x3fv, "glUniformMatrix4x3fv"));
    }

    // Desktop OpenGL 3.0
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 0))
    {
        (LoadProc(&ColorMaski, "glColorMaski"));
        (LoadProc(&GetBooleani_v, "glGetBooleani_v"));
        (LoadProc(&GetIntegeri_v, "glGetIntegeri_v"));
        (LoadProc(&Enablei, "glEnablei"));
        (LoadProc(&Disablei, "glDisablei"));
        (LoadProc(&IsEnabledi, "glIsEnabledi"));
        (LoadProc(&BeginTransformFeedback, "glBeginTransformFeedback"));
        (LoadProc(&EndTransformFeedback, "glEndTransformFeedback"));
        (LoadProc(&BindBufferRange, "glBindBufferRange"));
        (LoadProc(&BindBufferBase, "glBindBufferBase"));
        (LoadProc(&TransformFeedbackVaryings, "glTransformFeedbackVaryings"));
        (LoadProc(&GetTransformFeedbackVarying, "glGetTransformFeedbackVarying"));
        (LoadProc(&ClampColor, "glClampColor"));
        (LoadProc(&BeginConditionalRender, "glBeginConditionalRender"));
        (LoadProc(&EndConditionalRender, "glEndConditionalRender"));
        (LoadProc(&VertexAttribIPointer, "glVertexAttribIPointer"));
        (LoadProc(&GetVertexAttribIiv, "glGetVertexAttribIiv"));
        (LoadProc(&GetVertexAttribIuiv, "glGetVertexAttribIuiv"));
        (LoadProc(&VertexAttribI1i, "glVertexAttribI1i"));
        (LoadProc(&VertexAttribI2i, "glVertexAttribI2i"));
        (LoadProc(&VertexAttribI3i, "glVertexAttribI3i"));
        (LoadProc(&VertexAttribI4i, "glVertexAttribI4i"));
        (LoadProc(&VertexAttribI1ui, "glVertexAttribI1ui"));
        (LoadProc(&VertexAttribI2ui, "glVertexAttribI2ui"));
        (LoadProc(&VertexAttribI3ui, "glVertexAttribI3ui"));
        (LoadProc(&VertexAttribI4ui, "glVertexAttribI4ui"));
        (LoadProc(&VertexAttribI1iv, "glVertexAttribI1iv"));
        (LoadProc(&VertexAttribI2iv, "glVertexAttribI2iv"));
        (LoadProc(&VertexAttribI3iv, "glVertexAttribI3iv"));
        (LoadProc(&VertexAttribI4iv, "glVertexAttribI4iv"));
        (LoadProc(&VertexAttribI1uiv, "glVertexAttribI1uiv"));
        (LoadProc(&VertexAttribI2uiv, "glVertexAttribI2uiv"));
        (LoadProc(&VertexAttribI3uiv, "glVertexAttribI3uiv"));
        (LoadProc(&VertexAttribI4uiv, "glVertexAttribI4uiv"));
        (LoadProc(&VertexAttribI4bv, "glVertexAttribI4bv"));
        (LoadProc(&VertexAttribI4sv, "glVertexAttribI4sv"));
        (LoadProc(&VertexAttribI4ubv, "glVertexAttribI4ubv"));
        (LoadProc(&VertexAttribI4usv, "glVertexAttribI4usv"));
        (LoadProc(&GetUniformuiv, "glGetUniformuiv"));
        (LoadProc(&BindFragDataLocation, "glBindFragDataLocation"));
        (LoadProc(&GetFragDataLocation, "glGetFragDataLocation"));
        (LoadProc(&Uniform1ui, "glUniform1ui"));
        (LoadProc(&Uniform2ui, "glUniform2ui"));
        (LoadProc(&Uniform3ui, "glUniform3ui"));
        (LoadProc(&Uniform4ui, "glUniform4ui"));
        (LoadProc(&Uniform1uiv, "glUniform1uiv"));
        (LoadProc(&Uniform2uiv, "glUniform2uiv"));
        (LoadProc(&Uniform3uiv, "glUniform3uiv"));
        (LoadProc(&Uniform4uiv, "glUniform4uiv"));
        (LoadProc(&TexParameterIiv, "glTexParameterIiv"));
        (LoadProc(&TexParameterIuiv, "glTexParameterIuiv"));
        (LoadProc(&GetTexParameterIiv, "glGetTexParameterIiv"));
        (LoadProc(&GetTexParameterIuiv, "glGetTexParameterIuiv"));
        (LoadProc(&ClearBufferiv, "glClearBufferiv"));
        (LoadProc(&ClearBufferuiv, "glClearBufferuiv"));
        (LoadProc(&ClearBufferfv, "glClearBufferfv"));
        (LoadProc(&ClearBufferfi, "glClearBufferfi"));
        (LoadProc(&GetStringi, "glGetStringi"));
        (LoadProc(&IsRenderbuffer, "glIsRenderbuffer"));
        (LoadProc(&BindRenderbuffer, "glBindRenderbuffer"));
        (LoadProc(&DeleteRenderbuffers, "glDeleteRenderbuffers"));
        (LoadProc(&GenRenderbuffers, "glGenRenderbuffers"));
        (LoadProc(&RenderbufferStorage, "glRenderbufferStorage"));
        (LoadProc(&GetRenderbufferParameteriv, "glGetRenderbufferParameteriv"));
        (LoadProc(&IsFramebuffer, "glIsFramebuffer"));
        (LoadProc(&BindFramebuffer, "glBindFramebuffer"));
        (LoadProc(&DeleteFramebuffers, "glDeleteFramebuffers"));
        (LoadProc(&GenFramebuffers, "glGenFramebuffers"));
        (LoadProc(&CheckFramebufferStatus, "glCheckFramebufferStatus"));
        (LoadProc(&FramebufferTexture1D, "glFramebufferTexture1D"));
        (LoadProc(&FramebufferTexture2D, "glFramebufferTexture2D"));
        (LoadProc(&FramebufferTexture3D, "glFramebufferTexture3D"));
        (LoadProc(&FramebufferRenderbuffer, "glFramebufferRenderbuffer"));
        (LoadProc(&GetFramebufferAttachmentParameteriv, "glGetFramebufferAttachmentParameteriv"));
        (LoadProc(&GenerateMipmap, "glGenerateMipmap"));
        (LoadProc(&BlitFramebuffer, "glBlitFramebuffer"));
        (LoadProc(&RenderbufferStorageMultisample, "glRenderbufferStorageMultisample"));
        (LoadProc(&FramebufferTextureLayer, "glFramebufferTextureLayer"));
        (LoadProc(&MapBufferRange, "glMapBufferRange"));
        (LoadProc(&FlushMappedBufferRange, "glFlushMappedBufferRange"));
        (LoadProc(&BindVertexArray, "glBindVertexArray"));
        (LoadProc(&DeleteVertexArrays, "glDeleteVertexArrays"));
        (LoadProc(&GenVertexArrays, "glGenVertexArrays"));
        (LoadProc(&IsVertexArray, "glIsVertexArray"));
    }

    // Desktop OpenGL 3.1
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 1))
    {
        (LoadProc(&DrawArraysInstanced, "glDrawArraysInstanced"));
        (LoadProc(&DrawElementsInstanced, "glDrawElementsInstanced"));
        (LoadProc(&TexBuffer, "glTexBuffer"));
        (LoadProc(&PrimitiveRestartIndex, "glPrimitiveRestartIndex"));
        (LoadProc(&CopyBufferSubData, "glCopyBufferSubData"));
        (LoadProc(&GetUniformIndices, "glGetUniformIndices"));
        (LoadProc(&GetActiveUniformsiv, "glGetActiveUniformsiv"));
        (LoadProc(&GetActiveUniformName, "glGetActiveUniformName"));
        (LoadProc(&GetUniformBlockIndex, "glGetUniformBlockIndex"));
        (LoadProc(&GetActiveUniformBlockiv, "glGetActiveUniformBlockiv"));
        (LoadProc(&GetActiveUniformBlockName, "glGetActiveUniformBlockName"));
        (LoadProc(&UniformBlockBinding, "glUniformBlockBinding"));
        (LoadProc(&BindBufferRange, "glBindBufferRange"));
        (LoadProc(&BindBufferBase, "glBindBufferBase"));
        (LoadProc(&GetIntegeri_v, "glGetIntegeri_v"));
    }

    // Desktop OpenGL 3.2
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 2))
    {
        (LoadProc(&DrawElementsBaseVertex, "glDrawElementsBaseVertex"));
        (LoadProc(&DrawRangeElementsBaseVertex, "glDrawRangeElementsBaseVertex"));
        (LoadProc(&DrawElementsInstancedBaseVertex, "glDrawElementsInstancedBaseVertex"));
        (LoadProc(&MultiDrawElementsBaseVertex, "glMultiDrawElementsBaseVertex"));
        (LoadProc(&ProvokingVertex, "glProvokingVertex"));
        (LoadProc(&FenceSync, "glFenceSync"));
        (LoadProc(&IsSync, "glIsSync"));
        (LoadProc(&DeleteSync, "glDeleteSync"));
        (LoadProc(&ClientWaitSync, "glClientWaitSync"));
        (LoadProc(&WaitSync, "glWaitSync"));
        (LoadProc(&GetInteger64v, "glGetInteger64v"));
        (LoadProc(&GetSynciv, "glGetSynciv"));
        (LoadProc(&GetInteger64i_v, "glGetInteger64i_v"));
        (LoadProc(&GetBufferParameteri64v, "glGetBufferParameteri64v"));
        (LoadProc(&FramebufferTexture, "glFramebufferTexture"));
        (LoadProc(&TexImage2DMultisample, "glTexImage2DMultisample"));
        (LoadProc(&TexImage3DMultisample, "glTexImage3DMultisample"));
        (LoadProc(&GetMultisamplefv, "glGetMultisamplefv"));
        (LoadProc(&SampleMaski, "glSampleMaski"));
    }

    // Desktop OpenGL 3.3
    if (majorVersion > 3 || (majorVersion == 3 && minorVersion >= 3))
    {
        (LoadProc(&BindFragDataLocationIndexed, "glBindFragDataLocationIndexed"));
        (LoadProc(&GetFragDataIndex, "glGetFragDataIndex"));
        (LoadProc(&GenSamplers, "glGenSamplers"));
        (LoadProc(&DeleteSamplers, "glDeleteSamplers"));
        (LoadProc(&IsSampler, "glIsSampler"));
        (LoadProc(&BindSampler, "glBindSampler"));
        (LoadProc(&SamplerParameteri, "glSamplerParameteri"));
        (LoadProc(&SamplerParameteriv, "glSamplerParameteriv"));
        (LoadProc(&SamplerParameterf, "glSamplerParameterf"));
        (LoadProc(&SamplerParameterfv, "glSamplerParameterfv"));
        (LoadProc(&SamplerParameterIiv, "glSamplerParameterIiv"));
        (LoadProc(&SamplerParameterIuiv, "glSamplerParameterIuiv"));
        (LoadProc(&GetSamplerParameteriv, "glGetSamplerParameteriv"));
        (LoadProc(&GetSamplerParameterIiv, "glGetSamplerParameterIiv"));
        (LoadProc(&GetSamplerParameterfv, "glGetSamplerParameterfv"));
        (LoadProc(&GetSamplerParameterIuiv, "glGetSamplerParameterIuiv"));
        (LoadProc(&QueryCounter, "glQueryCounter"));
        (LoadProc(&GetQueryObjecti64v, "glGetQueryObjecti64v"));
        (LoadProc(&GetQueryObjectui64v, "glGetQueryObjectui64v"));
        (LoadProc(&VertexAttribDivisor, "glVertexAttribDivisor"));
        (LoadProc(&VertexAttribP1ui, "glVertexAttribP1ui"));
        (LoadProc(&VertexAttribP1uiv, "glVertexAttribP1uiv"));
        (LoadProc(&VertexAttribP2ui, "glVertexAttribP2ui"));
        (LoadProc(&VertexAttribP2uiv, "glVertexAttribP2uiv"));
        (LoadProc(&VertexAttribP3ui, "glVertexAttribP3ui"));
        (LoadProc(&VertexAttribP3uiv, "glVertexAttribP3uiv"));
        (LoadProc(&VertexAttribP4ui, "glVertexAttribP4ui"));
        (LoadProc(&VertexAttribP4uiv, "glVertexAttribP4uiv"));
        (LoadProc(&VertexP2ui, "glVertexP2ui"));
        (LoadProc(&VertexP2uiv, "glVertexP2uiv"));
        (LoadProc(&VertexP3ui, "glVertexP3ui"));
        (LoadProc(&VertexP3uiv, "glVertexP3uiv"));
        (LoadProc(&VertexP4ui, "glVertexP4ui"));
        (LoadProc(&VertexP4uiv, "glVertexP4uiv"));
        (LoadProc(&TexCoordP1ui, "glTexCoordP1ui"));
        (LoadProc(&TexCoordP1uiv, "glTexCoordP1uiv"));
        (LoadProc(&TexCoordP2ui, "glTexCoordP2ui"));
        (LoadProc(&TexCoordP2uiv, "glTexCoordP2uiv"));
        (LoadProc(&TexCoordP3ui, "glTexCoordP3ui"));
        (LoadProc(&TexCoordP3uiv, "glTexCoordP3uiv"));
        (LoadProc(&TexCoordP4ui, "glTexCoordP4ui"));
        (LoadProc(&TexCoordP4uiv, "glTexCoordP4uiv"));
        (LoadProc(&MultiTexCoordP1ui, "glMultiTexCoordP1ui"));
        (LoadProc(&MultiTexCoordP1uiv, "glMultiTexCoordP1uiv"));
        (LoadProc(&MultiTexCoordP2ui, "glMultiTexCoordP2ui"));
        (LoadProc(&MultiTexCoordP2uiv, "glMultiTexCoordP2uiv"));
        (LoadProc(&MultiTexCoordP3ui, "glMultiTexCoordP3ui"));
        (LoadProc(&MultiTexCoordP3uiv, "glMultiTexCoordP3uiv"));
        (LoadProc(&MultiTexCoordP4ui, "glMultiTexCoordP4ui"));
        (LoadProc(&MultiTexCoordP4uiv, "glMultiTexCoordP4uiv"));
        (LoadProc(&NormalP3ui, "glNormalP3ui"));
        (LoadProc(&NormalP3uiv, "glNormalP3uiv"));
        (LoadProc(&ColorP3ui, "glColorP3ui"));
        (LoadProc(&ColorP3uiv, "glColorP3uiv"));
        (LoadProc(&ColorP4ui, "glColorP4ui"));
        (LoadProc(&ColorP4uiv, "glColorP4uiv"));
        (LoadProc(&SecondaryColorP3ui, "glSecondaryColorP3ui"));
        (LoadProc(&SecondaryColorP3uiv, "glSecondaryColorP3uiv"));
    }

    // Desktop OpenGL 4.0
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 0))
    {
        (LoadProc(&MinSampleShading, "glMinSampleShading"));
        (LoadProc(&BlendEquationi, "glBlendEquationi"));
        (LoadProc(&BlendEquationSeparatei, "glBlendEquationSeparatei"));
        (LoadProc(&BlendFunci, "glBlendFunci"));
        (LoadProc(&BlendFuncSeparatei, "glBlendFuncSeparatei"));
        (LoadProc(&DrawArraysIndirect, "glDrawArraysIndirect"));
        (LoadProc(&DrawElementsIndirect, "glDrawElementsIndirect"));
        (LoadProc(&Uniform1d, "glUniform1d"));
        (LoadProc(&Uniform2d, "glUniform2d"));
        (LoadProc(&Uniform3d, "glUniform3d"));
        (LoadProc(&Uniform4d, "glUniform4d"));
        (LoadProc(&Uniform1dv, "glUniform1dv"));
        (LoadProc(&Uniform2dv, "glUniform2dv"));
        (LoadProc(&Uniform3dv, "glUniform3dv"));
        (LoadProc(&Uniform4dv, "glUniform4dv"));
        (LoadProc(&UniformMatrix2dv, "glUniformMatrix2dv"));
        (LoadProc(&UniformMatrix3dv, "glUniformMatrix3dv"));
        (LoadProc(&UniformMatrix4dv, "glUniformMatrix4dv"));
        (LoadProc(&UniformMatrix2x3dv, "glUniformMatrix2x3dv"));
        (LoadProc(&UniformMatrix2x4dv, "glUniformMatrix2x4dv"));
        (LoadProc(&UniformMatrix3x2dv, "glUniformMatrix3x2dv"));
        (LoadProc(&UniformMatrix3x4dv, "glUniformMatrix3x4dv"));
        (LoadProc(&UniformMatrix4x2dv, "glUniformMatrix4x2dv"));
        (LoadProc(&UniformMatrix4x3dv, "glUniformMatrix4x3dv"));
        (LoadProc(&GetUniformdv, "glGetUniformdv"));
        (LoadProc(&GetSubroutineUniformLocation, "glGetSubroutineUniformLocation"));
        (LoadProc(&GetSubroutineIndex, "glGetSubroutineIndex"));
        (LoadProc(&GetActiveSubroutineUniformiv, "glGetActiveSubroutineUniformiv"));
        (LoadProc(&GetActiveSubroutineUniformName, "glGetActiveSubroutineUniformName"));
        (LoadProc(&GetActiveSubroutineName, "glGetActiveSubroutineName"));
        (LoadProc(&UniformSubroutinesuiv, "glUniformSubroutinesuiv"));
        (LoadProc(&GetUniformSubroutineuiv, "glGetUniformSubroutineuiv"));
        (LoadProc(&GetProgramStageiv, "glGetProgramStageiv"));
        (LoadProc(&PatchParameteri, "glPatchParameteri"));
        (LoadProc(&PatchParameterfv, "glPatchParameterfv"));
        (LoadProc(&BindTransformFeedback, "glBindTransformFeedback"));
        (LoadProc(&DeleteTransformFeedbacks, "glDeleteTransformFeedbacks"));
        (LoadProc(&GenTransformFeedbacks, "glGenTransformFeedbacks"));
        (LoadProc(&IsTransformFeedback, "glIsTransformFeedback"));
        (LoadProc(&PauseTransformFeedback, "glPauseTransformFeedback"));
        (LoadProc(&ResumeTransformFeedback, "glResumeTransformFeedback"));
        (LoadProc(&DrawTransformFeedback, "glDrawTransformFeedback"));
        (LoadProc(&DrawTransformFeedbackStream, "glDrawTransformFeedbackStream"));
        (LoadProc(&BeginQueryIndexed, "glBeginQueryIndexed"));
        (LoadProc(&EndQueryIndexed, "glEndQueryIndexed"));
        (LoadProc(&GetQueryIndexediv, "glGetQueryIndexediv"));
    }

    // Desktop OpenGL 4.1
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 1))
    {
        (LoadProc(&ReleaseShaderCompiler, "glReleaseShaderCompiler"));
        (LoadProc(&ShaderBinary, "glShaderBinary"));
        (LoadProc(&GetShaderPrecisionFormat, "glGetShaderPrecisionFormat"));
        (LoadProc(&DepthRangef, "glDepthRangef"));
        (LoadProc(&ClearDepthf, "glClearDepthf"));
        (LoadProc(&GetProgramBinary, "glGetProgramBinary"));
        (LoadProc(&ProgramBinary, "glProgramBinary"));
        (LoadProc(&ProgramParameteri, "glProgramParameteri"));
        (LoadProc(&UseProgramStages, "glUseProgramStages"));
        (LoadProc(&ActiveShaderProgram, "glActiveShaderProgram"));
        (LoadProc(&CreateShaderProgramv, "glCreateShaderProgramv"));
        (LoadProc(&BindProgramPipeline, "glBindProgramPipeline"));
        (LoadProc(&DeleteProgramPipelines, "glDeleteProgramPipelines"));
        (LoadProc(&GenProgramPipelines, "glGenProgramPipelines"));
        (LoadProc(&IsProgramPipeline, "glIsProgramPipeline"));
        (LoadProc(&GetProgramPipelineiv, "glGetProgramPipelineiv"));
        (LoadProc(&ProgramParameteri, "glProgramParameteri"));
        (LoadProc(&ProgramUniform1i, "glProgramUniform1i"));
        (LoadProc(&ProgramUniform1iv, "glProgramUniform1iv"));
        (LoadProc(&ProgramUniform1f, "glProgramUniform1f"));
        (LoadProc(&ProgramUniform1fv, "glProgramUniform1fv"));
        (LoadProc(&ProgramUniform1d, "glProgramUniform1d"));
        (LoadProc(&ProgramUniform1dv, "glProgramUniform1dv"));
        (LoadProc(&ProgramUniform1ui, "glProgramUniform1ui"));
        (LoadProc(&ProgramUniform1uiv, "glProgramUniform1uiv"));
        (LoadProc(&ProgramUniform2i, "glProgramUniform2i"));
        (LoadProc(&ProgramUniform2iv, "glProgramUniform2iv"));
        (LoadProc(&ProgramUniform2f, "glProgramUniform2f"));
        (LoadProc(&ProgramUniform2fv, "glProgramUniform2fv"));
        (LoadProc(&ProgramUniform2d, "glProgramUniform2d"));
        (LoadProc(&ProgramUniform2dv, "glProgramUniform2dv"));
        (LoadProc(&ProgramUniform2ui, "glProgramUniform2ui"));
        (LoadProc(&ProgramUniform2uiv, "glProgramUniform2uiv"));
        (LoadProc(&ProgramUniform3i, "glProgramUniform3i"));
        (LoadProc(&ProgramUniform3iv, "glProgramUniform3iv"));
        (LoadProc(&ProgramUniform3f, "glProgramUniform3f"));
        (LoadProc(&ProgramUniform3fv, "glProgramUniform3fv"));
        (LoadProc(&ProgramUniform3d, "glProgramUniform3d"));
        (LoadProc(&ProgramUniform3dv, "glProgramUniform3dv"));
        (LoadProc(&ProgramUniform3ui, "glProgramUniform3ui"));
        (LoadProc(&ProgramUniform3uiv, "glProgramUniform3uiv"));
        (LoadProc(&ProgramUniform4i, "glProgramUniform4i"));
        (LoadProc(&ProgramUniform4iv, "glProgramUniform4iv"));
        (LoadProc(&ProgramUniform4f, "glProgramUniform4f"));
        (LoadProc(&ProgramUniform4fv, "glProgramUniform4fv"));
        (LoadProc(&ProgramUniform4d, "glProgramUniform4d"));
        (LoadProc(&ProgramUniform4dv, "glProgramUniform4dv"));
        (LoadProc(&ProgramUniform4ui, "glProgramUniform4ui"));
        (LoadProc(&ProgramUniform4uiv, "glProgramUniform4uiv"));
        (LoadProc(&ProgramUniformMatrix2fv, "glProgramUniformMatrix2fv"));
        (LoadProc(&ProgramUniformMatrix3fv, "glProgramUniformMatrix3fv"));
        (LoadProc(&ProgramUniformMatrix4fv, "glProgramUniformMatrix4fv"));
        (LoadProc(&ProgramUniformMatrix2dv, "glProgramUniformMatrix2dv"));
        (LoadProc(&ProgramUniformMatrix3dv, "glProgramUniformMatrix3dv"));
        (LoadProc(&ProgramUniformMatrix4dv, "glProgramUniformMatrix4dv"));
        (LoadProc(&ProgramUniformMatrix2x3fv, "glProgramUniformMatrix2x3fv"));
        (LoadProc(&ProgramUniformMatrix3x2fv, "glProgramUniformMatrix3x2fv"));
        (LoadProc(&ProgramUniformMatrix2x4fv, "glProgramUniformMatrix2x4fv"));
        (LoadProc(&ProgramUniformMatrix4x2fv, "glProgramUniformMatrix4x2fv"));
        (LoadProc(&ProgramUniformMatrix3x4fv, "glProgramUniformMatrix3x4fv"));
        (LoadProc(&ProgramUniformMatrix4x3fv, "glProgramUniformMatrix4x3fv"));
        (LoadProc(&ProgramUniformMatrix2x3dv, "glProgramUniformMatrix2x3dv"));
        (LoadProc(&ProgramUniformMatrix3x2dv, "glProgramUniformMatrix3x2dv"));
        (LoadProc(&ProgramUniformMatrix2x4dv, "glProgramUniformMatrix2x4dv"));
        (LoadProc(&ProgramUniformMatrix4x2dv, "glProgramUniformMatrix4x2dv"));
        (LoadProc(&ProgramUniformMatrix3x4dv, "glProgramUniformMatrix3x4dv"));
        (LoadProc(&ProgramUniformMatrix4x3dv, "glProgramUniformMatrix4x3dv"));
        (LoadProc(&ValidateProgramPipeline, "glValidateProgramPipeline"));
        (LoadProc(&GetProgramPipelineInfoLog, "glGetProgramPipelineInfoLog"));
        (LoadProc(&VertexAttribL1d, "glVertexAttribL1d"));
        (LoadProc(&VertexAttribL2d, "glVertexAttribL2d"));
        (LoadProc(&VertexAttribL3d, "glVertexAttribL3d"));
        (LoadProc(&VertexAttribL4d, "glVertexAttribL4d"));
        (LoadProc(&VertexAttribL1dv, "glVertexAttribL1dv"));
        (LoadProc(&VertexAttribL2dv, "glVertexAttribL2dv"));
        (LoadProc(&VertexAttribL3dv, "glVertexAttribL3dv"));
        (LoadProc(&VertexAttribL4dv, "glVertexAttribL4dv"));
        (LoadProc(&VertexAttribLPointer, "glVertexAttribLPointer"));
        (LoadProc(&GetVertexAttribLdv, "glGetVertexAttribLdv"));
        (LoadProc(&ViewportArrayv, "glViewportArrayv"));
        (LoadProc(&ViewportIndexedf, "glViewportIndexedf"));
        (LoadProc(&ViewportIndexedfv, "glViewportIndexedfv"));
        (LoadProc(&ScissorArrayv, "glScissorArrayv"));
        (LoadProc(&ScissorIndexed, "glScissorIndexed"));
        (LoadProc(&ScissorIndexedv, "glScissorIndexedv"));
        (LoadProc(&DepthRangeArrayv, "glDepthRangeArrayv"));
        (LoadProc(&DepthRangeIndexed, "glDepthRangeIndexed"));
        (LoadProc(&GetFloati_v, "glGetFloati_v"));
        (LoadProc(&GetDoublei_v, "glGetDoublei_v"));
    }

    // Desktop OpenGL 4.2
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 2))
    {
        (LoadProc(&DrawArraysInstancedBaseInstance, "glDrawArraysInstancedBaseInstance"));
        (LoadProc(&DrawElementsInstancedBaseInstance, "glDrawElementsInstancedBaseInstance"));
        (LoadProc(&DrawElementsInstancedBaseVertexBaseInstance,
                  "glDrawElementsInstancedBaseVertexBaseInstance"));
        (LoadProc(&GetInternalformativ, "glGetInternalformativ"));
        (LoadProc(&GetActiveAtomicCounterBufferiv, "glGetActiveAtomicCounterBufferiv"));
        (LoadProc(&BindImageTexture, "glBindImageTexture"));
        (LoadProc(&MemoryBarrier, "glMemoryBarrier"));
        (LoadProc(&TexStorage1D, "glTexStorage1D"));
        (LoadProc(&TexStorage2D, "glTexStorage2D"));
        (LoadProc(&TexStorage3D, "glTexStorage3D"));
        (LoadProc(&DrawTransformFeedbackInstanced, "glDrawTransformFeedbackInstanced"));
        (LoadProc(&DrawTransformFeedbackStreamInstanced, "glDrawTransformFeedbackStreamInstanced"));
    }

    // Desktop OpenGL 4.3
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 3))
    {
        (LoadProc(&ClearBufferData, "glClearBufferData"));
        (LoadProc(&ClearBufferSubData, "glClearBufferSubData"));
        (LoadProc(&DispatchCompute, "glDispatchCompute"));
        (LoadProc(&DispatchComputeIndirect, "glDispatchComputeIndirect"));
        (LoadProc(&CopyImageSubData, "glCopyImageSubData"));
        (LoadProc(&FramebufferParameteri, "glFramebufferParameteri"));
        (LoadProc(&GetFramebufferParameteriv, "glGetFramebufferParameteriv"));
        (LoadProc(&GetInternalformati64v, "glGetInternalformati64v"));
        (LoadProc(&InvalidateTexSubImage, "glInvalidateTexSubImage"));
        (LoadProc(&InvalidateTexImage, "glInvalidateTexImage"));
        (LoadProc(&InvalidateBufferSubData, "glInvalidateBufferSubData"));
        (LoadProc(&InvalidateBufferData, "glInvalidateBufferData"));
        (LoadProc(&InvalidateFramebuffer, "glInvalidateFramebuffer"));
        (LoadProc(&InvalidateSubFramebuffer, "glInvalidateSubFramebuffer"));
        (LoadProc(&MultiDrawArraysIndirect, "glMultiDrawArraysIndirect"));
        (LoadProc(&MultiDrawElementsIndirect, "glMultiDrawElementsIndirect"));
        (LoadProc(&GetProgramInterfaceiv, "glGetProgramInterfaceiv"));
        (LoadProc(&GetProgramResourceIndex, "glGetProgramResourceIndex"));
        (LoadProc(&GetProgramResourceName, "glGetProgramResourceName"));
        (LoadProc(&GetProgramResourceiv, "glGetProgramResourceiv"));
        (LoadProc(&GetProgramResourceLocation, "glGetProgramResourceLocation"));
        (LoadProc(&GetProgramResourceLocationIndex, "glGetProgramResourceLocationIndex"));
        (LoadProc(&ShaderStorageBlockBinding, "glShaderStorageBlockBinding"));
        (LoadProc(&TexBufferRange, "glTexBufferRange"));
        (LoadProc(&TexStorage2DMultisample, "glTexStorage2DMultisample"));
        (LoadProc(&TexStorage3DMultisample, "glTexStorage3DMultisample"));
        (LoadProc(&TextureView, "glTextureView"));
        (LoadProc(&BindVertexBuffer, "glBindVertexBuffer"));
        (LoadProc(&VertexAttribFormat, "glVertexAttribFormat"));
        (LoadProc(&VertexAttribIFormat, "glVertexAttribIFormat"));
        (LoadProc(&VertexAttribLFormat, "glVertexAttribLFormat"));
        (LoadProc(&VertexAttribBinding, "glVertexAttribBinding"));
        (LoadProc(&VertexBindingDivisor, "glVertexBindingDivisor"));
        (LoadProc(&DebugMessageControl, "glDebugMessageControl"));
        (LoadProc(&DebugMessageInsert, "glDebugMessageInsert"));
        (LoadProc(&DebugMessageCallback, "glDebugMessageCallback"));
        (LoadProc(&GetDebugMessageLog, "glGetDebugMessageLog"));
        (LoadProc(&PushDebugGroup, "glPushDebugGroup"));
        (LoadProc(&PopDebugGroup, "glPopDebugGroup"));
        (LoadProc(&ObjectLabel, "glObjectLabel"));
        (LoadProc(&GetObjectLabel, "glGetObjectLabel"));
        (LoadProc(&ObjectPtrLabel, "glObjectPtrLabel"));
        (LoadProc(&GetObjectPtrLabel, "glGetObjectPtrLabel"));
    }

    // Desktop OpenGL 4.4
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 4))
    {
        (LoadProc(&BufferStorage, "glBufferStorage"));
        (LoadProc(&ClearTexImage, "glClearTexImage"));
        (LoadProc(&ClearTexSubImage, "glClearTexSubImage"));
        (LoadProc(&BindBuffersBase, "glBindBuffersBase"));
        (LoadProc(&BindBuffersRange, "glBindBuffersRange"));
        (LoadProc(&BindTextures, "glBindTextures"));
        (LoadProc(&BindSamplers, "glBindSamplers"));
        (LoadProc(&BindImageTextures, "glBindImageTextures"));
        (LoadProc(&BindVertexBuffers, "glBindVertexBuffers"));
    }

    // Desktop OpenGL 4.5
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 5))
    {
        (LoadProc(&ClipControl, "glClipControl"));
        (LoadProc(&CreateTransformFeedbacks, "glCreateTransformFeedbacks"));
        (LoadProc(&TransformFeedbackBufferBase, "glTransformFeedbackBufferBase"));
        (LoadProc(&TransformFeedbackBufferRange, "glTransformFeedbackBufferRange"));
        (LoadProc(&GetTransformFeedbackiv, "glGetTransformFeedbackiv"));
        (LoadProc(&GetTransformFeedbacki_v, "glGetTransformFeedbacki_v"));
        (LoadProc(&GetTransformFeedbacki64_v, "glGetTransformFeedbacki64_v"));
        (LoadProc(&CreateBuffers, "glCreateBuffers"));
        (LoadProc(&NamedBufferStorage, "glNamedBufferStorage"));
        (LoadProc(&NamedBufferData, "glNamedBufferData"));
        (LoadProc(&NamedBufferSubData, "glNamedBufferSubData"));
        (LoadProc(&CopyNamedBufferSubData, "glCopyNamedBufferSubData"));
        (LoadProc(&ClearNamedBufferData, "glClearNamedBufferData"));
        (LoadProc(&ClearNamedBufferSubData, "glClearNamedBufferSubData"));
        (LoadProc(&MapNamedBuffer, "glMapNamedBuffer"));
        (LoadProc(&MapNamedBufferRange, "glMapNamedBufferRange"));
        (LoadProc(&UnmapNamedBuffer, "glUnmapNamedBuffer"));
        (LoadProc(&FlushMappedNamedBufferRange, "glFlushMappedNamedBufferRange"));
        (LoadProc(&GetNamedBufferParameteriv, "glGetNamedBufferParameteriv"));
        (LoadProc(&GetNamedBufferParameteri64v, "glGetNamedBufferParameteri64v"));
        (LoadProc(&GetNamedBufferPointerv, "glGetNamedBufferPointerv"));
        (LoadProc(&GetNamedBufferSubData, "glGetNamedBufferSubData"));
        (LoadProc(&CreateFramebuffers, "glCreateFramebuffers"));
        (LoadProc(&NamedFramebufferRenderbuffer, "glNamedFramebufferRenderbuffer"));
        (LoadProc(&NamedFramebufferParameteri, "glNamedFramebufferParameteri"));
        (LoadProc(&NamedFramebufferTexture, "glNamedFramebufferTexture"));
        (LoadProc(&NamedFramebufferTextureLayer, "glNamedFramebufferTextureLayer"));
        (LoadProc(&NamedFramebufferDrawBuffer, "glNamedFramebufferDrawBuffer"));
        (LoadProc(&NamedFramebufferDrawBuffers, "glNamedFramebufferDrawBuffers"));
        (LoadProc(&NamedFramebufferReadBuffer, "glNamedFramebufferReadBuffer"));
        (LoadProc(&InvalidateNamedFramebufferData, "glInvalidateNamedFramebufferData"));
        (LoadProc(&InvalidateNamedFramebufferSubData, "glInvalidateNamedFramebufferSubData"));
        (LoadProc(&ClearNamedFramebufferiv, "glClearNamedFramebufferiv"));
        (LoadProc(&ClearNamedFramebufferuiv, "glClearNamedFramebufferuiv"));
        (LoadProc(&ClearNamedFramebufferfv, "glClearNamedFramebufferfv"));
        (LoadProc(&ClearNamedFramebufferfi, "glClearNamedFramebufferfi"));
        (LoadProc(&BlitNamedFramebuffer, "glBlitNamedFramebuffer"));
        (LoadProc(&CheckNamedFramebufferStatus, "glCheckNamedFramebufferStatus"));
        (LoadProc(&GetNamedFramebufferParameteriv, "glGetNamedFramebufferParameteriv"));
        (LoadProc(&GetNamedFramebufferAttachmentParameteriv,
                  "glGetNamedFramebufferAttachmentParameteriv"));
        (LoadProc(&CreateRenderbuffers, "glCreateRenderbuffers"));
        (LoadProc(&NamedRenderbufferStorage, "glNamedRenderbufferStorage"));
        (LoadProc(&NamedRenderbufferStorageMultisample, "glNamedRenderbufferStorageMultisample"));
        (LoadProc(&GetNamedRenderbufferParameteriv, "glGetNamedRenderbufferParameteriv"));
        (LoadProc(&CreateTextures, "glCreateTextures"));
        (LoadProc(&TextureBuffer, "glTextureBuffer"));
        (LoadProc(&TextureBufferRange, "glTextureBufferRange"));
        (LoadProc(&TextureStorage1D, "glTextureStorage1D"));
        (LoadProc(&TextureStorage2D, "glTextureStorage2D"));
        (LoadProc(&TextureStorage3D, "glTextureStorage3D"));
        (LoadProc(&TextureStorage2DMultisample, "glTextureStorage2DMultisample"));
        (LoadProc(&TextureStorage3DMultisample, "glTextureStorage3DMultisample"));
        (LoadProc(&TextureSubImage1D, "glTextureSubImage1D"));
        (LoadProc(&TextureSubImage2D, "glTextureSubImage2D"));
        (LoadProc(&TextureSubImage3D, "glTextureSubImage3D"));
        (LoadProc(&CompressedTextureSubImage1D, "glCompressedTextureSubImage1D"));
        (LoadProc(&CompressedTextureSubImage2D, "glCompressedTextureSubImage2D"));
        (LoadProc(&CompressedTextureSubImage3D, "glCompressedTextureSubImage3D"));
        (LoadProc(&CopyTextureSubImage1D, "glCopyTextureSubImage1D"));
        (LoadProc(&CopyTextureSubImage2D, "glCopyTextureSubImage2D"));
        (LoadProc(&CopyTextureSubImage3D, "glCopyTextureSubImage3D"));
        (LoadProc(&TextureParameterf, "glTextureParameterf"));
        (LoadProc(&TextureParameterfv, "glTextureParameterfv"));
        (LoadProc(&TextureParameteri, "glTextureParameteri"));
        (LoadProc(&TextureParameterIiv, "glTextureParameterIiv"));
        (LoadProc(&TextureParameterIuiv, "glTextureParameterIuiv"));
        (LoadProc(&TextureParameteriv, "glTextureParameteriv"));
        (LoadProc(&GenerateTextureMipmap, "glGenerateTextureMipmap"));
        (LoadProc(&BindTextureUnit, "glBindTextureUnit"));
        (LoadProc(&GetTextureImage, "glGetTextureImage"));
        (LoadProc(&GetCompressedTextureImage, "glGetCompressedTextureImage"));
        (LoadProc(&GetTextureLevelParameterfv, "glGetTextureLevelParameterfv"));
        (LoadProc(&GetTextureLevelParameteriv, "glGetTextureLevelParameteriv"));
        (LoadProc(&GetTextureParameterfv, "glGetTextureParameterfv"));
        (LoadProc(&GetTextureParameterIiv, "glGetTextureParameterIiv"));
        (LoadProc(&GetTextureParameterIuiv, "glGetTextureParameterIuiv"));
        (LoadProc(&GetTextureParameteriv, "glGetTextureParameteriv"));
        (LoadProc(&CreateVertexArrays, "glCreateVertexArrays"));
        (LoadProc(&DisableVertexArrayAttrib, "glDisableVertexArrayAttrib"));
        (LoadProc(&EnableVertexArrayAttrib, "glEnableVertexArrayAttrib"));
        (LoadProc(&VertexArrayElementBuffer, "glVertexArrayElementBuffer"));
        (LoadProc(&VertexArrayVertexBuffer, "glVertexArrayVertexBuffer"));
        (LoadProc(&VertexArrayVertexBuffers, "glVertexArrayVertexBuffers"));
        (LoadProc(&VertexArrayAttribBinding, "glVertexArrayAttribBinding"));
        (LoadProc(&VertexArrayAttribFormat, "glVertexArrayAttribFormat"));
        (LoadProc(&VertexArrayAttribIFormat, "glVertexArrayAttribIFormat"));
        (LoadProc(&VertexArrayAttribLFormat, "glVertexArrayAttribLFormat"));
        (LoadProc(&VertexArrayBindingDivisor, "glVertexArrayBindingDivisor"));
        (LoadProc(&GetVertexArrayiv, "glGetVertexArrayiv"));
        (LoadProc(&GetVertexArrayIndexediv, "glGetVertexArrayIndexediv"));
        (LoadProc(&GetVertexArrayIndexed64iv, "glGetVertexArrayIndexed64iv"));
        (LoadProc(&CreateSamplers, "glCreateSamplers"));
        (LoadProc(&CreateProgramPipelines, "glCreateProgramPipelines"));
        (LoadProc(&CreateQueries, "glCreateQueries"));
        (LoadProc(&GetQueryBufferObjecti64v, "glGetQueryBufferObjecti64v"));
        (LoadProc(&GetQueryBufferObjectiv, "glGetQueryBufferObjectiv"));
        (LoadProc(&GetQueryBufferObjectui64v, "glGetQueryBufferObjectui64v"));
        (LoadProc(&GetQueryBufferObjectuiv, "glGetQueryBufferObjectuiv"));
        (LoadProc(&MemoryBarrierByRegion, "glMemoryBarrierByRegion"));
        (LoadProc(&GetTextureSubImage, "glGetTextureSubImage"));
        (LoadProc(&GetCompressedTextureSubImage, "glGetCompressedTextureSubImage"));
        (LoadProc(&GetGraphicsResetStatus, "glGetGraphicsResetStatus"));
        (LoadProc(&GetnCompressedTexImage, "glGetnCompressedTexImage"));
        (LoadProc(&GetnTexImage, "glGetnTexImage"));
        (LoadProc(&GetnUniformdv, "glGetnUniformdv"));
        (LoadProc(&GetnUniformfv, "glGetnUniformfv"));
        (LoadProc(&GetnUniformiv, "glGetnUniformiv"));
        (LoadProc(&GetnUniformuiv, "glGetnUniformuiv"));
        (LoadProc(&ReadnPixels, "glReadnPixels"));
        (LoadProc(&GetnMapdv, "glGetnMapdv"));
        (LoadProc(&GetnMapfv, "glGetnMapfv"));
        (LoadProc(&GetnMapiv, "glGetnMapiv"));
        (LoadProc(&GetnPixelMapfv, "glGetnPixelMapfv"));
        (LoadProc(&GetnPixelMapuiv, "glGetnPixelMapuiv"));
        (LoadProc(&GetnPixelMapusv, "glGetnPixelMapusv"));
        (LoadProc(&GetnPolygonStipple, "glGetnPolygonStipple"));
        (LoadProc(&GetnColorTable, "glGetnColorTable"));
        (LoadProc(&GetnConvolutionFilter, "glGetnConvolutionFilter"));
        (LoadProc(&GetnSeparableFilter, "glGetnSeparableFilter"));
        (LoadProc(&GetnHistogram, "glGetnHistogram"));
        (LoadProc(&GetnMinmax, "glGetnMinmax"));
        (LoadProc(&TextureBarrier, "glTextureBarrier"));
    }

    // Desktop OpenGL 4.6
    if (majorVersion > 4 || (majorVersion == 4 && minorVersion >= 6))
    {
        (LoadProc(&SpecializeShader, "glSpecializeShader"));
        (LoadProc(&MultiDrawArraysIndirectCount, "glMultiDrawArraysIndirectCount"));
        (LoadProc(&MultiDrawElementsIndirectCount, "glMultiDrawElementsIndirectCount"));
        (LoadProc(&PolygonOffsetClamp, "glPolygonOffsetClamp"));
    }

    return true;
}
}  // namespace aquarium