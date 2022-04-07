

#include "express-gpu/glv3_context.h"

#include "glad/glad.h"
#include "express-gpu/egl_window.h"

static void g_buffer_map_destroy(gpointer data);

static void g_vao_point_data_destroy(gpointer data);

GHashTable *program_is_external_map = NULL;

GHashTable *program_data_map = NULL;

GHashTable *to_external_texture_id_map = NULL;

static void g_program_data_destroy(gpointer data)
{
    GLchar *program_data = (GLchar *)data;
    g_free(program_data);
}

int pixel_size_calc(GLenum format, GLenum type)
{
    switch (type)
    {
    case GL_BYTE:
        switch (format)
        {
        case GL_R8:
        case GL_R8I:
        case GL_R8_SNORM:
        case GL_RED:
            return sizeof(char);
        case GL_RED_INTEGER:
            return sizeof(char);
        case GL_RG8:
        case GL_RG8I:
        case GL_RG8_SNORM:
        case GL_RG:
            return sizeof(char) * 2;
        case GL_RG_INTEGER:
            return sizeof(char) * 2;
        case GL_RGB8:
        case GL_RGB8I:
        case GL_RGB8_SNORM:
        case GL_RGB:
            return sizeof(char) * 3;
        case GL_RGB_INTEGER:
            return sizeof(char) * 3;
        case GL_RGBA8:
        case GL_RGBA8I:
        case GL_RGBA8_SNORM:
        case GL_RGBA:
            return sizeof(char) * 4;
        case GL_RGBA_INTEGER:
            return sizeof(char) * 4;
        }
        break;
    case GL_UNSIGNED_BYTE:
        switch (format)
        {
        case GL_R8:
        case GL_R8UI:
        case GL_RED:
            return sizeof(unsigned char);
        case GL_RED_INTEGER:
            return sizeof(unsigned char);
        case GL_ALPHA8_EXT:
        case GL_ALPHA:
            return sizeof(unsigned char);
        case GL_LUMINANCE8_EXT:
        case GL_LUMINANCE:
            return sizeof(unsigned char);
        case GL_LUMINANCE8_ALPHA8_EXT:
        case GL_LUMINANCE_ALPHA:
            return sizeof(unsigned char) * 2;
        case GL_RG8:
        case GL_RG8UI:
        case GL_RG:
            return sizeof(unsigned char) * 2;
        case GL_RG_INTEGER:
            return sizeof(unsigned char) * 2;
        case GL_RGB8:
        case GL_RGB8UI:
        case GL_SRGB8:
        case GL_RGB:
            return sizeof(unsigned char) * 3;
        case GL_RGB_INTEGER:
            return sizeof(unsigned char) * 3;
        case GL_RGBA8:
        case GL_RGBA8UI:
        case GL_SRGB8_ALPHA8:
        case GL_RGBA:
            return sizeof(unsigned char) * 4;
        case GL_RGBA_INTEGER:
            return sizeof(unsigned char) * 4;
        case GL_BGRA_EXT:

            return sizeof(unsigned char) * 4;
        }
        break;
    case GL_SHORT:
        switch (format)
        {
        case GL_R16I:
        case GL_RED_INTEGER:
            return sizeof(short);
        case GL_RG16I:
        case GL_RG_INTEGER:
            return sizeof(short) * 2;
        case GL_RGB16I:
        case GL_RGB_INTEGER:
            return sizeof(short) * 3;
        case GL_RGBA16I:
        case GL_RGBA_INTEGER:
            return sizeof(short) * 4;
        }
        break;
    case GL_UNSIGNED_SHORT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT:
            return sizeof(unsigned short);
        case GL_R16UI:
        case GL_RED_INTEGER:
            return sizeof(unsigned short);
        case GL_RG16UI:
        case GL_RG_INTEGER:
            return sizeof(unsigned short) * 2;
        case GL_RGB16UI:
        case GL_RGB_INTEGER:
            return sizeof(unsigned short) * 3;
        case GL_RGBA16UI:
        case GL_RGBA_INTEGER:
            return sizeof(unsigned short) * 4;
        }
        break;
    case GL_INT:
        switch (format)
        {
        case GL_R32I:
        case GL_RED_INTEGER:
            return sizeof(int);
        case GL_RG32I:
        case GL_RG_INTEGER:
            return sizeof(int) * 2;
        case GL_RGB32I:
        case GL_RGB_INTEGER:
            return sizeof(int) * 3;
        case GL_RGBA32I:
        case GL_RGBA_INTEGER:
            return sizeof(int) * 4;
        }
        break;
    case GL_UNSIGNED_INT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT16:
        case GL_DEPTH_COMPONENT24:

        case GL_DEPTH_COMPONENT:
            return sizeof(unsigned int);
        case GL_R32UI:
        case GL_RED_INTEGER:
            return sizeof(unsigned int);
        case GL_RG32UI:
        case GL_RG_INTEGER:
            return sizeof(unsigned int) * 2;
        case GL_RGB32UI:
        case GL_RGB_INTEGER:
            return sizeof(unsigned int) * 3;
        case GL_RGBA32UI:
        case GL_RGBA_INTEGER:
            return sizeof(unsigned int) * 4;
        }
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_5_6_5:

        return sizeof(unsigned short);
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_2_10_10_10_REV:

        return sizeof(unsigned int);
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        return sizeof(float) + sizeof(unsigned int);
    case GL_FLOAT:
        switch (format)
        {
        case GL_DEPTH_COMPONENT32F:
        case GL_DEPTH_COMPONENT:
            return sizeof(float);

        case GL_ALPHA:
            return sizeof(float);

        case GL_LUMINANCE:
            return sizeof(float);

        case GL_LUMINANCE_ALPHA:
            return sizeof(float) * 2;
        case GL_RED:
            return sizeof(float);
        case GL_R32F:
            return sizeof(float);
        case GL_RG:
            return sizeof(float) * 2;
        case GL_RG32F:
            return sizeof(float) * 2;
        case GL_RGB:
            return sizeof(float) * 3;
        case GL_RGB32F:
            return sizeof(float) * 3;
        case GL_RGBA:
            return sizeof(float) * 4;
        case GL_RGBA32F:
            return sizeof(float) * 4;
        }
        break;
    case GL_HALF_FLOAT:

        switch (format)
        {

        case GL_ALPHA:
            return sizeof(unsigned short);

        case GL_LUMINANCE:
            return sizeof(unsigned short);

        case GL_LUMINANCE_ALPHA:
            return sizeof(unsigned short) * 2;
        case GL_RED:
            return sizeof(unsigned short);
        case GL_R16F:
            return sizeof(unsigned short);
        case GL_RG:
            return sizeof(unsigned short) * 2;
        case GL_RG16F:
            return sizeof(unsigned short) * 2;
        case GL_RGB:
            return sizeof(unsigned short) * 3;
        case GL_RGB16F:
            return sizeof(unsigned short) * 3;
        case GL_RGBA:
            return sizeof(unsigned short) * 4;
        case GL_RGBA16F:
            return sizeof(unsigned short) * 4;
        }
        break;
    }

    return 0;
}

size_t gl_sizeof(GLenum type)
{
    size_t retval = 0;
    switch (type)
    {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        retval = 1;
        break;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_HALF_FLOAT:

        retval = 2;
        break;
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_FIXED:
    case GL_BOOL:
        retval = 4;
        break;
#ifdef GL_DOUBLE
    case GL_DOUBLE:
        retval = 8;
        break;
    case GL_DOUBLE_VEC2:
        retval = 16;
        break;
    case GL_DOUBLE_VEC3:
        retval = 24;
        break;
    case GL_DOUBLE_VEC4:
        retval = 32;
        break;
    case GL_DOUBLE_MAT2:
        retval = 8 * 4;
        break;
    case GL_DOUBLE_MAT3:
        retval = 8 * 9;
        break;
    case GL_DOUBLE_MAT4:
        retval = 8 * 16;
        break;
    case GL_DOUBLE_MAT2x3:
    case GL_DOUBLE_MAT3x2:
        retval = 8 * 6;
        break;
    case GL_DOUBLE_MAT2x4:
    case GL_DOUBLE_MAT4x2:
        retval = 8 * 8;
        break;
    case GL_DOUBLE_MAT3x4:
    case GL_DOUBLE_MAT4x3:
        retval = 8 * 12;
        break;
#endif
    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_BOOL_VEC2:
        retval = 8;
        break;
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_BOOL_VEC3:
    case GL_FLOAT_VEC3:
        retval = 12;
        break;
    case GL_FLOAT_VEC4:
    case GL_BOOL_VEC4:
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_FLOAT_MAT2:
        retval = 16;
        break;
    case GL_FLOAT_MAT3:
        retval = 36;
        break;
    case GL_FLOAT_MAT4:
        retval = 64;
        break;
    case GL_FLOAT_MAT2x3:
    case GL_FLOAT_MAT3x2:
        retval = 4 * 6;
        break;
    case GL_FLOAT_MAT2x4:
    case GL_FLOAT_MAT4x2:
        retval = 4 * 8;
        break;
    case GL_FLOAT_MAT3x4:
    case GL_FLOAT_MAT4x3:
        retval = 4 * 12;
        break;
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_IMAGE_CUBE:
    case GL_IMAGE_2D_ARRAY:
    case GL_INT_IMAGE_2D:
    case GL_INT_IMAGE_3D:
    case GL_INT_IMAGE_CUBE:
    case GL_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D:
    case GL_UNSIGNED_INT_IMAGE_3D:
    case GL_UNSIGNED_INT_IMAGE_CUBE:
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_ATOMIC_COUNTER:
        retval = 4;
        break;
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_5_6_5:

        retval = 2;
        break;
    case GL_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
    case GL_UNSIGNED_INT_5_9_9_9_REV:
    case GL_UNSIGNED_INT_2_10_10_10_REV:

        retval = 4;
        break;
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV:
        retval = 4 + 4;
        break;
    default:
        printf("**** ERROR unknown type 0x%x (%s,%d)\n", type, __FUNCTION__, __LINE__);
        retval = 4;
    }
    return retval;
}

size_t gl_pname_size(GLenum pname)
{
    size_t s = 0;

    switch (pname)
    {
    case GL_DEPTH_TEST:
    case GL_DEPTH_FUNC:
    case GL_DEPTH_BITS:
    case GL_MAX_CLIP_PLANES:
    case GL_MAX_COLOR_ATTACHMENTS:
    case GL_MAX_DRAW_BUFFERS:
    case GL_GREEN_BITS:
    case GL_MAX_MODELVIEW_STACK_DEPTH:
    case GL_MAX_PROJECTION_STACK_DEPTH:
    case GL_MAX_TEXTURE_STACK_DEPTH:
    case GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES:
    case GL_IMPLEMENTATION_COLOR_READ_TYPE_OES:

    case GL_MAX_TEXTURE_SIZE:

    case GL_TEXTURE_ENV_MODE:
    case GL_FOG_MODE:
    case GL_FOG_DENSITY:
    case GL_FOG_START:
    case GL_FOG_END:
    case GL_SPOT_EXPONENT:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
    case GL_SHININESS:
    case GL_LIGHT_MODEL_TWO_SIDE:
    case GL_POINT_SIZE:
    case GL_POINT_SIZE_MIN:
    case GL_POINT_SIZE_MAX:
    case GL_POINT_FADE_THRESHOLD_SIZE:
    case GL_CULL_FACE:
    case GL_CULL_FACE_MODE:
    case GL_FRONT_FACE:
    case GL_SHADE_MODEL:
    case GL_DEPTH_WRITEMASK:
    case GL_DEPTH_CLEAR_VALUE:
    case GL_STENCIL_FAIL:
    case GL_STENCIL_PASS_DEPTH_FAIL:
    case GL_STENCIL_PASS_DEPTH_PASS:
    case GL_STENCIL_REF:
    case GL_STENCIL_WRITEMASK:
    case GL_MATRIX_MODE:
    case GL_MODELVIEW_STACK_DEPTH:
    case GL_PROJECTION_STACK_DEPTH:
    case GL_TEXTURE_STACK_DEPTH:
    case GL_ALPHA_TEST_FUNC:
    case GL_ALPHA_TEST_REF:
    case GL_ALPHA_TEST:
    case GL_DITHER:
    case GL_BLEND_DST:
    case GL_BLEND_SRC:
    case GL_BLEND:
    case GL_LOGIC_OP_MODE:
    case GL_SCISSOR_TEST:
    case GL_MAX_TEXTURE_UNITS:
    case GL_ACTIVE_TEXTURE:
    case GL_ALPHA_BITS:
    case GL_ARRAY_BUFFER_BINDING:
    case GL_BLUE_BITS:
    case GL_CLIENT_ACTIVE_TEXTURE:
    case GL_CLIP_PLANE0:
    case GL_CLIP_PLANE1:
    case GL_CLIP_PLANE2:
    case GL_CLIP_PLANE3:
    case GL_CLIP_PLANE4:
    case GL_CLIP_PLANE5:
    case GL_COLOR_ARRAY:
    case GL_COLOR_ARRAY_BUFFER_BINDING:
    case GL_COLOR_ARRAY_SIZE:
    case GL_COLOR_ARRAY_STRIDE:
    case GL_COLOR_ARRAY_TYPE:
    case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL:
    case GL_PACK_ALIGNMENT:
    case GL_PERSPECTIVE_CORRECTION_HINT:

    case GL_POINT_SMOOTH:
    case GL_POINT_SMOOTH_HINT:

    case GL_COMBINE_ALPHA:
    case GL_SRC0_RGB:
    case GL_SRC1_RGB:
    case GL_SRC2_RGB:
    case GL_OPERAND0_RGB:
    case GL_OPERAND1_RGB:
    case GL_OPERAND2_RGB:
    case GL_SRC0_ALPHA:
    case GL_SRC1_ALPHA:
    case GL_SRC2_ALPHA:
    case GL_OPERAND0_ALPHA:
    case GL_OPERAND1_ALPHA:
    case GL_OPERAND2_ALPHA:
    case GL_RGB_SCALE:
    case GL_ALPHA_SCALE:
    case GL_COMBINE_RGB:
    case GL_POLYGON_OFFSET_FACTOR:
    case GL_POLYGON_OFFSET_FILL:
    case GL_POLYGON_OFFSET_UNITS:
    case GL_RED_BITS:
    case GL_RESCALE_NORMAL:
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
    case GL_SAMPLE_ALPHA_TO_ONE:
    case GL_SAMPLE_BUFFERS:
    case GL_SAMPLE_COVERAGE:
    case GL_SAMPLE_COVERAGE_INVERT:
    case GL_SAMPLE_COVERAGE_VALUE:
    case GL_SAMPLES:
    case GL_MAX_SAMPLES_EXT:
    case GL_STENCIL_BITS:
    case GL_STENCIL_CLEAR_VALUE:
    case GL_STENCIL_FUNC:
    case GL_STENCIL_TEST:
    case GL_STENCIL_VALUE_MASK:
    case GL_STENCIL_BACK_FUNC:
    case GL_STENCIL_BACK_VALUE_MASK:
    case GL_STENCIL_BACK_REF:
    case GL_STENCIL_BACK_FAIL:
    case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
    case GL_STENCIL_BACK_PASS_DEPTH_PASS:
    case GL_STENCIL_BACK_WRITEMASK:
    case GL_TEXTURE_2D:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_BINDING_2D:
    case GL_TEXTURE_BINDING_CUBE_MAP:

    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COORD_ARRAY:
    case GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING:
    case GL_TEXTURE_COORD_ARRAY_SIZE:
    case GL_TEXTURE_COORD_ARRAY_STRIDE:
    case GL_TEXTURE_COORD_ARRAY_TYPE:
    case GL_TEXTURE_IMMUTABLE_FORMAT:
    case GL_UNPACK_ALIGNMENT:
    case GL_VERTEX_ARRAY:
    case GL_VERTEX_ARRAY_BUFFER_BINDING:
    case GL_VERTEX_ARRAY_SIZE:
    case GL_VERTEX_ARRAY_STRIDE:
    case GL_VERTEX_ARRAY_TYPE:
    case GL_SPOT_CUTOFF:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
    case GL_GENERATE_MIPMAP:
    case GL_GENERATE_MIPMAP_HINT:

    case GL_RENDERBUFFER_BINDING:
    case GL_FRAMEBUFFER_BINDING:

    case GL_FENCE_STATUS_NV:
    case GL_FENCE_CONDITION_NV:

    case GL_MAX_LIGHTS:
    case GL_SHADER_TYPE:
    case GL_COMPILE_STATUS:
    case GL_SHADER_SOURCE_LENGTH:
    case GL_CURRENT_PROGRAM:
    case GL_SUBPIXEL_BITS:
    case GL_MAX_3D_TEXTURE_SIZE:
    case GL_MAX_ARRAY_TEXTURE_LAYERS:
    case GL_MAX_CUBE_MAP_TEXTURE_SIZE:

    case GL_SHADER_COMPILER:
    case GL_MAX_VERTEX_ATTRIBS:
    case GL_MAX_VERTEX_UNIFORM_VECTORS:
    case GL_MAX_VARYING_VECTORS:
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
    case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
    case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
    case GL_MAX_RENDERBUFFER_SIZE:
    case GL_MAX_TEXTURE_IMAGE_UNITS:

    case GL_LINE_WIDTH:
    case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS:
    case GL_MAX_UNIFORM_BUFFER_BINDINGS:
    case GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS:
    case GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS:
    case GL_UNIFORM_BLOCK_BINDING:
    case GL_UNIFORM_BLOCK_DATA_SIZE:
    case GL_UNIFORM_BLOCK_NAME_LENGTH:
    case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:
    case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
    case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER:
    case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
    case GL_CURRENT_QUERY:
    case GL_QUERY_RESULT:
    case GL_QUERY_RESULT_AVAILABLE:
    case GL_READ_BUFFER:

    case GL_ACTIVE_ATOMIC_COUNTER_BUFFERS:
    case GL_ACTIVE_ATTRIBUTES:
    case GL_ACTIVE_ATTRIBUTE_MAX_LENGTH:
    case GL_ACTIVE_UNIFORM_BLOCKS:
    case GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH:
    case GL_ACTIVE_UNIFORMS:
    case GL_ACTIVE_UNIFORM_MAX_LENGTH:
    case GL_ATTACHED_SHADERS:
    case GL_DELETE_STATUS:
    case GL_INFO_LOG_LENGTH:
    case GL_LINK_STATUS:
    case GL_PROGRAM_BINARY_LENGTH:
    case GL_PROGRAM_BINARY_RETRIEVABLE_HINT:
    case GL_PROGRAM_SEPARABLE:
    case GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT:
    case GL_TRANSFORM_FEEDBACK_BUFFER_MODE:
    case GL_TRANSFORM_FEEDBACK_VARYINGS:
    case GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH:
    case GL_TRANSFORM_FEEDBACK_ACTIVE:
#define GL_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS 0x8C8A
#define GL_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS 0x8C80
    case GL_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS:
    case GL_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS:
    case GL_VALIDATE_STATUS:
        s = 1;
        break;
    case GL_ALIASED_LINE_WIDTH_RANGE:
    case GL_ALIASED_POINT_SIZE_RANGE:
    case GL_DEPTH_RANGE:
    case GL_MAX_VIEWPORT_DIMS:
    case GL_SMOOTH_POINT_SIZE_RANGE:
    case GL_SMOOTH_LINE_WIDTH_RANGE:
    case GL_SAMPLE_POSITION:
        s = 2;
        break;
    case GL_SPOT_DIRECTION:
    case GL_POINT_DISTANCE_ATTENUATION:
    case GL_CURRENT_NORMAL:
    case GL_COMPUTE_WORK_GROUP_SIZE:
        s = 3;
        break;
    case GL_CURRENT_VERTEX_ATTRIB:
    case GL_CURRENT_TEXTURE_COORDS:
    case GL_CURRENT_COLOR:
    case GL_FOG_COLOR:
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_EMISSION:
    case GL_POSITION:
    case GL_LIGHT_MODEL_AMBIENT:
    case GL_TEXTURE_ENV_COLOR:
    case GL_SCISSOR_BOX:
    case GL_VIEWPORT:

    case GL_COLOR_CLEAR_VALUE:
    case GL_COLOR_WRITEMASK:
    case GL_AMBIENT_AND_DIFFUSE:
    case GL_BLEND_COLOR:
        s = 4;
        break;
    case GL_MODELVIEW_MATRIX:
    case GL_PROJECTION_MATRIX:
    case GL_TEXTURE_MATRIX:
        s = 16;
        break;

    case GL_MAX_ELEMENTS_VERTICES:
    case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
    case GL_MAX_VERTEX_UNIFORM_BLOCKS:
    case GL_MAX_VERTEX_OUTPUT_COMPONENTS:
    case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
    case GL_MAX_FRAGMENT_UNIFORM_BLOCKS:
    case GL_MAX_FRAGMENT_INPUT_COMPONENTS:
    case GL_MIN_PROGRAM_TEXEL_OFFSET:
    case GL_MAX_PROGRAM_TEXEL_OFFSET:
    case GL_MAX_COMBINED_UNIFORM_BLOCKS:
    case GL_MAX_VARYING_COMPONENTS:
    case GL_NUM_EXTENSIONS:
    case GL_VERTEX_ARRAY_BINDING:
    case GL_BLEND_SRC_RGB:
    case GL_BLEND_SRC_ALPHA:
    case GL_BLEND_DST_RGB:
    case GL_BLEND_DST_ALPHA:
    case GL_BLEND_EQUATION:
    case GL_BLEND_EQUATION_ALPHA:
    case GL_DRAW_BUFFER0:
    case GL_DRAW_BUFFER1:
    case GL_DRAW_BUFFER2:
    case GL_DRAW_BUFFER3:
    case GL_DRAW_BUFFER4:
    case GL_DRAW_BUFFER5:
    case GL_DRAW_BUFFER6:
    case GL_DRAW_BUFFER7:
    case GL_DRAW_BUFFER8:
    case GL_DRAW_BUFFER9:
    case GL_DRAW_BUFFER10:
    case GL_DRAW_BUFFER11:
    case GL_DRAW_BUFFER12:
    case GL_DRAW_BUFFER13:
    case GL_DRAW_BUFFER14:
    case GL_DRAW_BUFFER15:
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
    case GL_TRANSFORM_FEEDBACK_BUFFER_START:
    case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE:
    case GL_TRANSFORM_FEEDBACK_PAUSED:
    case GL_ACTIVE_RESOURCES:
    case GL_MAX_IMAGE_UNITS:
        s = 1;
        break;
    case GL_MAX_ELEMENT_INDEX:
        s = 2;
        break;
    case GL_MAX_TEXTURE_LOD_BIAS:
        s = 1;
        break;
    case GL_MAX_ELEMENTS_INDICES:
        s = 2;
        break;
    case GL_MAX_SERVER_WAIT_TIMEOUT:
        s = 2;
        break;
    case GL_MAX_UNIFORM_BLOCK_SIZE:
        s = 2;
        break;
    case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
        s = 2;
        break;
    case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
        s = 2;
        break;
    case GL_PROGRAM_BINARY_FORMATS:
        s = 8;
        break;
    case GL_NUM_PROGRAM_BINARY_FORMATS:
        s = 1;
        break;
    case GL_SHADER_BINARY_FORMATS:
        s = 8;
        break;
    case GL_NUM_SHADER_BINARY_FORMATS:
        s = 1;
        break;
    case GL_COMPRESSED_TEXTURE_FORMATS:
        s = 128;
        break;
    case GL_NUM_COMPRESSED_TEXTURE_FORMATS:
        s = 1;
        break;
    default:
        printf("gl_pname_size: unknow pname 0x%08x\n", pname);
        s = 1;
    }
    return s;
}

void d_glBindFramebuffer_special(void *context, GLenum target, GLuint framebuffer)
{
    GLuint draw_fbo0 = ((Opengl_Context *)context)->draw_fbo0;
    GLuint read_fbo0 = ((Opengl_Context *)context)->read_fbo0;

    if (framebuffer == 0)
    {
        if (target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {

            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_fbo0);
        }
        if (target == GL_READ_FRAMEBUFFER || target == GL_FRAMEBUFFER)
        {

            glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo0);
        }
    }
    else
    {
        glBindFramebuffer(target, framebuffer);
    }
}

void d_glBindBuffer_origin(void *context, GLenum target, GLuint buffer)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    Buffer_Status *status = &(opengl_context->bound_buffer_status.buffer_status);

    switch (target)
    {
    case GL_ARRAY_BUFFER:
        status->array_buffer = buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        status->element_array_buffer = buffer;
        opengl_context->bound_buffer_status.attrib_point->element_array_buffer = buffer;
        break;
    case GL_COPY_READ_BUFFER:
        status->copy_read_buffer = buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        status->copy_write_buffer = buffer;
        break;
    case GL_PIXEL_PACK_BUFFER:
        status->pixel_pack_buffer = buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        status->pixel_unpack_buffer = buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        status->transform_feedback_buffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        status->uniform_buffer = buffer;
        break;
    case GL_ATOMIC_COUNTER_BUFFER:
        status->atomic_counter_buffer = buffer;
        break;
    case GL_DISPATCH_INDIRECT_BUFFER:
        status->dispatch_indirect_buffer = buffer;
        break;
    case GL_DRAW_INDIRECT_BUFFER:
        status->draw_indirect_buffer = buffer;
        break;
    case GL_SHADER_STORAGE_BUFFER:
        status->shader_storage_buffer = buffer;
    }

    glBindBuffer(target, buffer);
}

int init_program_data(GLuint program)
{
    GLint link_status = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &link_status);

    int buf_len;
    GLchar *program_data;

    if (link_status == 0)
    {
        return 0;
    }
    else
    {

        GLint uniform_num = 0;
        GLint attrib_num = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_num);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &attrib_num);

        GLint max_uniform_name_len = 0;
        GLint max_attrib_name_len = 0;
        GLint max_uniform_block_name_len = 0;

        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_len);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attrib_name_len);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_uniform_block_name_len);

        GLint uniform_blocks_num = 0;
        GLint transform_feedback_varyings = 0;
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &uniform_blocks_num);
        glGetProgramiv(program, GL_TRANSFORM_FEEDBACK_VARYINGS, &transform_feedback_varyings);

        int name_len = max_uniform_name_len > max_attrib_name_len ? max_uniform_name_len : max_attrib_name_len;
        name_len = name_len > max_uniform_block_name_len ? name_len : max_uniform_block_name_len;

        name_len += 1;

        GLchar *name_buf = g_malloc(name_len);

        buf_len = (name_len + 3 * 4) * (uniform_num + attrib_num + uniform_blocks_num) + 4 * 7 + name_len;
        program_data = g_malloc(buf_len);

        GLchar *temp_ptr = program_data;
        int *int_ptr = (int *)temp_ptr;

        *int_ptr = uniform_num;
        *(int_ptr + 1) = attrib_num;
        *(int_ptr + 2) = uniform_blocks_num;
        *(int_ptr + 3) = max_uniform_name_len;
        *(int_ptr + 4) = max_attrib_name_len;
        *(int_ptr + 5) = max_uniform_block_name_len;
        *(int_ptr + 6) = transform_feedback_varyings;

        temp_ptr += 7 * sizeof(int);

        GLint size;
        GLenum type;
        GLint location;
        int has_image = 0;

        for (int i = 0; i < uniform_num; i++)
        {
            glGetActiveUniform(program, i, name_len, NULL, &size, &type, name_buf);

            location = glGetUniformLocation(program, name_buf);

            if (strstr(name_buf, "has_EGL_image_external") != NULL)
            {
                if (program_is_external_map == NULL)
                {
                    program_is_external_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(program_is_external_map, program, (gpointer)1);
                has_image = 1;
                continue;
            }

            int_ptr = temp_ptr;

            *int_ptr = i - has_image;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = type;
            *(int_ptr + 3) = location;

            temp_ptr += 4 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;
        }

        for (int i = 0; i < attrib_num; i++)
        {
            glGetActiveAttrib(program, i, name_len, NULL, &size, &type, name_buf);

            location = glGetAttribLocation(program, name_buf);

            int_ptr = temp_ptr;

            *int_ptr = i;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = type;
            *(int_ptr + 3) = location;

            temp_ptr += 4 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;
        }

        int uniform_block_active_uniforms;
        for (int i = 0; i < uniform_blocks_num; i++)
        {
            int_ptr = temp_ptr;
            glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniform_block_active_uniforms);
            glGetActiveUniformBlockiv(program, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
            glGetActiveUniformBlockName(program, i, name_len, NULL, name_buf);

            *int_ptr = i;
            *(int_ptr + 1) = size;
            *(int_ptr + 2) = uniform_block_active_uniforms;

            temp_ptr += 3 * sizeof(int);
            strncpy(temp_ptr, name_buf, name_len);
            temp_ptr += strlen(name_buf) + 1;
        }

        if (has_image)
        {
            *((int *)program_data) = uniform_num - 1;
        }

        if (program_data_map == NULL)
        {
            program_data_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_program_data_destroy);
        }

        g_hash_table_insert(program_data_map, GINT_TO_POINTER(program), program_data);

        if (buf_len > temp_ptr - program_data + 10)
        {
            buf_len = temp_ptr - program_data + 10;
        }
        g_free(name_buf);

        return buf_len;
    }
}

void d_glProgramBinary_special(void *context, GLuint program, GLenum binaryFormat, const void *binary, GLsizei length, int *program_data_len)
{

    glProgramBinary(program, binaryFormat, binary, length);

    *program_data_len = init_program_data(program);
    return;
}

void d_glLinkProgram_special(void *context, GLuint program, int *program_data_len)
{

    glLinkProgram(program);

    *program_data_len = init_program_data(program);
    return;
}

void d_glGetProgramData(void *context, GLuint program, int buf_len, void *program_data)
{
    Guest_Mem *guest_mem = (Guest_Mem *)program_data;

    if (program_data_map == NULL || program == 0)
    {

        printf("error! program_data_map %llx program %d", program_data_map, program);
        return;
    }

    GLchar *save_program_data = g_hash_table_lookup(program_data_map, GINT_TO_POINTER(program));

    if (save_program_data == NULL)
    {
        printf("error! save_program_data NULL program %u", program);
        return;
    }

    guest_read(guest_mem, save_program_data, 0, buf_len);

    g_hash_table_remove(program_data_map, GINT_TO_POINTER(program));

    return;
}

void get_default_out(char *string, char *out)
{
    out[0] = 0;
    int out_loc = 0;
    char *out_name = NULL;
    char *out_type = NULL;
    while (*string != ';')
        string++;
    string--;
    while (isspace(*string))
        string--;
    while (!isspace(*string))
        string--;

    out_name = string + 1;
    for (int i = 0; out_name[i] != ';' && !isspace(out_name[i]) && out_name[i] != 0; i++)
    {
        out[out_loc] = out_name[i];
        out_loc++;
    }
    out[out_loc] = '=';
    out_loc++;

    while (isspace(*string))
        string--;

    while (!isspace(*string))
        string--;
    out_type = string + 1;

    if (out_type[3] == '4')
    {
        strcpy(out + out_loc, "vec4(0,0,0,0);return;}");
    }
    else if (out_type[3] == '3')
    {
        strcpy(out + out_loc, "vec3(0,0,0);return;}");
    }
    else
    {
        strcpy(out + out_loc, "0;return;}");
    }
}

void d_glShaderSource_special(void *context, GLuint shader, GLsizei count, GLint *length, const GLchar **string)
{
    static const char DEFAULT_VERSION[] = "#version 330\n";
    static const char SHADOW_SAMPLER_EXTENSION[] = "#extension GL_NV_shadow_samplers_cube : enable\n";
    static const char USE_EXTERNAL_UNIFORM[] = "if(has_EGL_image_external==1){";

    int has_find_external = 0;
    int has_version = 0;
    int has_texturecube = 0;
    char *new_string1 = NULL;
    char *new_string2 = NULL;

    for (int i = 0; i < count; i++)
    {
        char *string_loc = strstr(string[i], "has_EGL_image_external");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_find_external = 1;
        }
        string_loc = strstr(string[i], "#version");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_version = 1;
        }
        string_loc = strstr(string[i], "textureCube");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            has_texturecube = 1;
        }
        string_loc = strstr(string[i], "gl_FragDepthEXT");
        if (string_loc != NULL && string_loc - string[i] <= length[i])
        {
            string_loc[12] = ' ';
            string_loc[13] = ' ';
            string_loc[14] = ' ';
        }
    }

    if (!has_version)
    {
        new_string1 = g_malloc(length[0] + sizeof(DEFAULT_VERSION) + sizeof(SHADOW_SAMPLER_EXTENSION));
        int loc = 0;
        memcpy(new_string1, DEFAULT_VERSION, sizeof(DEFAULT_VERSION) - 1);
        loc += sizeof(DEFAULT_VERSION) - 1;
        if (has_texturecube)
        {
            memcpy(new_string1 + loc, SHADOW_SAMPLER_EXTENSION, sizeof(SHADOW_SAMPLER_EXTENSION) - 1);
            loc += sizeof(SHADOW_SAMPLER_EXTENSION) - 1;
        }
        memcpy(new_string1 + loc, string[0], length[0]);
        loc += length[0];
        new_string1[loc] = 0;
        length[0] = loc;
        string[0] = new_string1;
    }

    GLint shader_type;
    glGetShaderiv(shader, GL_SHADER_TYPE, &shader_type);

    if (has_find_external == 1 && shader_type == GL_FRAGMENT_SHADER)
    {
        char *out_loc = NULL;
        for (int i = 0; i < count; i++)
        {
            out_loc = strstr(string[i], "out ");
            if (out_loc != NULL && (unsigned long long)(out_loc - string[i]) <= (unsigned long long)length[i])
            {
                if (isspace(*(out_loc - 1)))
                {
                    break;
                }
            }
        }
        if (out_loc == NULL)
        {
            out_loc = "out vec4 gl_FragColor;";
        }
        char default_out_string[200];

        get_default_out(out_loc, default_out_string);

        for (int i = 0; i < count; i++)
        {
            char *string_loc = strstr(string[i], "main(void)");
            if (string_loc == NULL)
            {
                string_loc = strstr(string[i], "main()");
            }

            if (string_loc != NULL && (unsigned long long)(string_loc - string[i]) <= (unsigned long long)length[i])
            {

                while (string_loc[0] != '{' && (unsigned long long)(string_loc - string[i]) <= (unsigned long long)length[i])
                {
                    string_loc++;
                }
                string_loc++;
                if (string_loc - string[i] > length[i])
                {
                    break;
                }
                has_find_external = 1;
                new_string2 = g_malloc(length[i] + sizeof(USE_EXTERNAL_UNIFORM) - 1 + strlen(default_out_string));
                int offset = 0;
                memcpy(new_string2 + offset, string[i], string_loc - string[i]);
                offset += (string_loc - string[i]);
                memcpy(new_string2 + offset, USE_EXTERNAL_UNIFORM, sizeof(USE_EXTERNAL_UNIFORM) - 1);
                offset += sizeof(USE_EXTERNAL_UNIFORM) - 1;
                memcpy(new_string2 + offset, default_out_string, strlen(default_out_string));
                offset += strlen(default_out_string);
                memcpy(new_string2 + offset, string_loc, length[i] - (string_loc - string[i]));
                offset += length[i] - (string_loc - string[i]);

                length[i] = offset;
                string[i] = new_string2;
            }
        }
    }

    glShaderSource(shader, count, string, length);

    if (new_string1 != NULL)
    {
        g_free(new_string1);
    }
    if (new_string2 != NULL)
    {
        g_free(new_string2);
    }
}

void d_glGetString_special(void *context, GLenum name, GLubyte *buffer)
{
    const GLubyte *static_string = glGetString(name);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetString string too long %x %s", name, static_string);
    }
    memcpy(buffer, static_string, len);
}

void d_glGetStringi_special(void *context, GLenum name, GLuint index, GLubyte *buffer)
{
    const GLubyte *static_string = glGetStringi(name, index);
    express_printf("getStringi index %u:%s\n", index, static_string);
    int len = strlen((const char *)static_string);
    if (len >= 1024)
    {
        len = 1023;
        printf("error, glGetStringi string too long %x %u %s", name, index, static_string);
    }
    memcpy(buffer, static_string, len);
}

void d_glViewport_special(void *context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    Opengl_Context *real_opengl_context = (Opengl_Context *)context;
    real_opengl_context->view_x = 0;
    real_opengl_context->view_y = 0;
    real_opengl_context->view_w = width;
    real_opengl_context->view_h = height;
    glViewport(x, y, width, height);
    return;
}

void d_glEGLImageTargetTexture2DOES(void *context, GLenum target, GLeglImageOES image)
{

    return;
}

void d_glUseProgram_special(void *context, GLuint program)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    int ret = 0;
    if (program_is_external_map != NULL)
    {
        ret = g_hash_table_lookup(program_is_external_map, GINT_TO_POINTER(program));
    }

    if (ret == 1 && opengl_context->current_target == GL_TEXTURE_2D)
    {

        for (int i = 0; i < preload_static_context_value->max_combined_texture_image_units; i++)
        {
            if (opengl_context->current_texture_external != 0)
            {
                GLuint texture = g_hash_table_lookup(to_external_texture_id_map, (gpointer)(opengl_context->current_texture_external));
                if (texture != 0)
                {
                    if (opengl_context->current_active_texture != 0)
                    {
                        glActiveTexture(GL_TEXTURE0);
                    }
                    glBindTexture(GL_TEXTURE_2D, texture);
                    opengl_context->current_target = GL_TEXTURE_EXTERNAL_OES;

                    if (opengl_context->current_active_texture != 0)
                    {
                        glActiveTexture(opengl_context->current_active_texture + GL_TEXTURE0);
                    }

                    break;
                }
            }
        }
    }
    if (ret == 0 && opengl_context->current_target == GL_TEXTURE_EXTERNAL_OES)
    {

        glBindTexture(GL_TEXTURE_2D, opengl_context->current_texture_2D[opengl_context->current_active_texture]);
        opengl_context->current_target = GL_TEXTURE_2D;
    }

    glUseProgram(program);
}

void d_glBindEGLImage(void *context, GLenum target, GLeglImageOES image)
{
    Opengl_Context *opengl_context = (Opengl_Context *)context;
    uint64_t gbuffer_id = (uint64_t)image;
    Window_Buffer *real_surface = get_surface_from_gbuffer_id(gbuffer_id);
    EGL_Image *egl_image = get_image_from_gbuffer_id(gbuffer_id);

    if (real_surface != NULL && egl_image != NULL)
    {
        printf("error! real_surface and egl_image are not NULL!");
    }

    switch (target)
    {
    case GL_READ_ONLY:
    {
        if (real_surface != NULL)
        {
            acquire_texture_from_surface(real_surface);
            glBindTexture(GL_TEXTURE_2D, real_surface->fbo_texture[real_surface->now_acquired]);

            if (opengl_context->current_texture_external != 0)
            {
                if (to_external_texture_id_map == NULL)
                {
                    to_external_texture_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(to_external_texture_id_map, opengl_context->current_texture_external, GINT_TO_POINTER(real_surface->fbo_texture[real_surface->now_acquired]));
            }
        }
        if (egl_image != NULL)
        {
            init_image_texture(egl_image);

            acquire_texture_from_image(egl_image);
            opengl_context->bind_image = egl_image;

            if (opengl_context->current_texture_external != 0)
            {
                if (to_external_texture_id_map == NULL)
                {
                    to_external_texture_id_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, NULL);
                }
                g_hash_table_insert(to_external_texture_id_map, opengl_context->current_texture_external, GINT_TO_POINTER(egl_image->fbo_texture));
            }

            glBindTexture(GL_TEXTURE_2D, egl_image->fbo_texture);
        }
        break;
    }
    case GL_WRITE_ONLY:
    {
        if (real_surface != NULL)
        {
            glBindTexture(GL_TEXTURE_2D, real_surface->fbo_texture[real_surface->now_acquired]);
            printf("error! Surface is writen by image!");
        }

        if (egl_image != NULL && egl_image->target != EGL_GL_TEXTURE_2D)
        {
            if (opengl_context->draw_surface != NULL && opengl_context->draw_surface->I_am_composer == 0)
            {
                egl_image->need_reverse = 1;
            }

            egl_image->host_has_data = 1;
            init_image_fbo(egl_image, egl_image->need_reverse);

            glBindFramebuffer(GL_FRAMEBUFFER, egl_image->display_fbo);
        }
        break;
    }
    case GL_SYNC_FLUSH_COMMANDS_BIT:
    {
        if (real_surface != NULL)
        {
            release_texture_from_surface(real_surface);
        }
        if (egl_image != NULL)
        {
            release_texture_from_image(egl_image);
            opengl_context->bind_image = NULL;
        }
        break;
    }
    case GL_NONE:
    {
        if (egl_image != NULL)
        {
            opengl_context->bind_image = NULL;
            ATOMIC_SET_USED(egl_image->display_texture_is_use);
        }
    }
    default:
    {

        break;
    }
    }
}

void d_glEGLImageTargetRenderbufferStorageOES(void *context, GLenum target, GLeglImageOES image)
{
}

void resource_context_init(Resource_Context *resources, Share_Resources *share_resources)
{
    if (share_resources != NULL)
    {
        resources->share_resources = share_resources;
        resources->share_resources->counter += 1;
    }
    else
    {
        resources->share_resources = g_malloc(sizeof(Share_Resources));
        memset(resources->share_resources, 0, sizeof(Share_Resources));
        resources->share_resources->counter = 1;
    }

    resources->exclusive_resources = g_malloc(sizeof(Exclusive_Resources));
    memset(resources->exclusive_resources, 0, sizeof(Exclusive_Resources));

    resources->texture_resource = &(resources->share_resources->texture_resource);
    resources->buffer_resource = &(resources->share_resources->buffer_resource);
    resources->render_buffer_resource = &(resources->share_resources->render_buffer_resource);
    resources->sampler_resource = &(resources->share_resources->sample_resource);

    resources->shader_resource = &(resources->share_resources->shader_resource);
    resources->program_resource = &(resources->share_resources->program_resource);

    resources->sync_resource = &(resources->share_resources->sync_resource);

    resources->frame_buffer_resource = &(resources->exclusive_resources->frame_buffer_resource);
    resources->program_pipeline_resource = &(resources->exclusive_resources->program_pipeline_resource);
    resources->transform_feedback_resource = &(resources->exclusive_resources->transform_feedback_resource);
    resources->vertex_array_resource = &(resources->exclusive_resources->vertex_array_resource);

    resources->query_resource = &(resources->exclusive_resources->query_resource);
}

#define DESTROY_RESOURCES(resource_name, resource_delete)                                              \
    if (resources->resource_name->resource_id_map != NULL)                                             \
    {                                                                                                  \
        for (int i = 1; i <= resources->resource_name->max_id; i++)                                    \
        {                                                                                              \
            if (resources->resource_name->resource_id_map[i] == 0)                                     \
                continue;                                                                              \
            if (now_delete_len < 1000)                                                                 \
            {                                                                                          \
                delete_buffers[now_delete_len] = (GLuint)resources->resource_name->resource_id_map[i]; \
                now_delete_len += 1;                                                                   \
            }                                                                                          \
            else                                                                                       \
            {                                                                                          \
                resource_delete(now_delete_len, delete_buffers);                                       \
                now_delete_len = 0;                                                                    \
            }                                                                                          \
        }                                                                                              \
        if (now_delete_len != 0)                                                                       \
        {                                                                                              \
            resource_delete(now_delete_len, delete_buffers);                                           \
            now_delete_len = 0;                                                                        \
        }                                                                                              \
        g_free(resources->resource_name->resource_id_map);                                             \
    }

void resource_context_destroy(Resource_Context *resources)
{
    resources->share_resources->counter -= 1;
    if (resources->share_resources->counter == 0)
    {
        GLuint delete_buffers[1000];
        GLuint now_delete_len = 0;
        DESTROY_RESOURCES(texture_resource, glDeleteTextures);
        DESTROY_RESOURCES(buffer_resource, glDeleteBuffers);
        DESTROY_RESOURCES(render_buffer_resource, glDeleteRenderbuffers);
        DESTROY_RESOURCES(sampler_resource, glDeleteSamplers);

        if (resources->shader_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->shader_resource->max_id; i++)
            {
                if (resources->shader_resource->resource_id_map[i] != 0)
                {
                    glDeleteShader((GLuint)resources->shader_resource->resource_id_map[i]);
                }
            }
            g_free(resources->shader_resource->resource_id_map);
        }

        if (resources->program_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->program_resource->max_id; i++)
            {
                if (resources->program_resource->resource_id_map[i] != 0)
                {
                    if (program_is_external_map != NULL)
                    {
                        g_hash_table_remove(program_is_external_map, GINT_TO_POINTER((GLuint)resources->program_resource->resource_id_map[i]));
                    }
                    if (program_data_map != NULL)
                    {
                        g_hash_table_remove(program_data_map, GINT_TO_POINTER((GLuint)resources->program_resource->resource_id_map[i]));
                    }
                    glDeleteProgram((GLuint)resources->program_resource->resource_id_map[i]);
                }
            }
            g_free(resources->program_resource->resource_id_map);
        }

        if (resources->sync_resource->resource_id_map != NULL)
        {
            for (int i = 1; i <= resources->sync_resource->max_id; i++)
            {
                if (resources->sync_resource->resource_id_map[i] != 0)
                {
                    glDeleteSync((GLsync)resources->sync_resource->resource_id_map[i]);
                }
            }
            g_free(resources->sync_resource->resource_id_map);
        }

        g_free(resources->share_resources);
    }

    g_free(resources->frame_buffer_resource->resource_id_map);
    g_free(resources->program_pipeline_resource->resource_id_map);
    g_free(resources->transform_feedback_resource->resource_id_map);
    g_free(resources->vertex_array_resource->resource_id_map);
    g_free(resources->query_resource->resource_id_map);

    g_free(resources->exclusive_resources);
}

Opengl_Context *opengl_context_create(Opengl_Context *share_context)
{
    Opengl_Context *opengl_context = g_malloc(sizeof(Opengl_Context));
    opengl_context->is_current = 0;
    opengl_context->need_destroy = 0;
    opengl_context->window = NULL;

    opengl_context->bind_image = NULL;

    opengl_context->current_texture_2D = g_malloc(sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);
    memset(opengl_context->current_texture_2D, 0, sizeof(GLuint) * preload_static_context_value->max_combined_texture_image_units);

    opengl_context->current_texture_external = 0;

    opengl_context->current_target = GL_TEXTURE_2D;
    opengl_context->current_active_texture = 0;

    opengl_context->view_x = 0;
    opengl_context->view_y = 0;
    opengl_context->view_w = 0;
    opengl_context->view_h = 0;

    send_message_to_main_window(MAIN_CREATE_CHILD_WINDOW, &(opengl_context->window));

    Share_Resources *share_resources = NULL;
    if (share_context != NULL)
    {
        share_resources = share_context->resource_status.share_resources;
    }

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    opengl_context->buffer_map = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_buffer_map_destroy);

    bound_buffer->vao_point_data = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_vao_point_data_destroy);

    Attrib_Point *temp_point = g_malloc(sizeof(Attrib_Point));
    memset(temp_point, 0, sizeof(Attrib_Point));

    g_hash_table_insert(bound_buffer->vao_point_data, GINT_TO_POINTER(0), (gpointer)temp_point);

    bound_buffer->attrib_point = temp_point;

    resource_context_init(&(opengl_context->resource_status), share_resources);

    bound_buffer->asyn_unpack_texture_buffer = 0;
    bound_buffer->asyn_pack_texture_buffer = 0;

    bound_buffer->has_init = 0;

    opengl_context->draw_fbo0 = 0;
    opengl_context->read_fbo0 = 0;

    return opengl_context;
}

void opengl_context_init(Opengl_Context *context)
{

    Bound_Buffer *bound_buffer = &(context->bound_buffer_status);
    if (bound_buffer->has_init == 0)
    {

        bound_buffer->has_init = 1;
        glGenBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glGenBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));

        glGenBuffers(1, &(bound_buffer->attrib_point->indices_buffer_object));
        glGenBuffers(MAX_VERTEX_ATTRIBS_NUM, bound_buffer->attrib_point->buffer_object);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        glViewport(context->view_x, context->view_y, context->view_w, context->view_h);
    }
}

void opengl_context_destroy(Opengl_Context *context)
{
    express_printf("opengl context destroy %lx\n", context);
    Opengl_Context *opengl_context = (Opengl_Context *)context;

    Bound_Buffer *bound_buffer = &(opengl_context->bound_buffer_status);

    g_hash_table_destroy(opengl_context->buffer_map);

    g_hash_table_destroy(bound_buffer->vao_point_data);

    if (bound_buffer->has_init == 1)
    {
        glDeleteBuffers(1, &(bound_buffer->asyn_unpack_texture_buffer));
        glDeleteBuffers(1, &(bound_buffer->asyn_pack_texture_buffer));
    }

    resource_context_destroy(&(opengl_context->resource_status));
}

static void g_buffer_map_destroy(gpointer data)
{
    express_printf("buffer_map destroy\n");
    Guest_Host_Map *map_res = (Guest_Host_Map *)data;
    g_free(map_res);
}

static void g_vao_point_data_destroy(gpointer data)
{
    Attrib_Point *vao_point = (Attrib_Point *)data;
    if (vao_point->indices_buffer_object != 0)
    {
        glDeleteBuffers(1, &(vao_point->indices_buffer_object));
        glDeleteBuffers(MAX_VERTEX_ATTRIBS_NUM, vao_point->buffer_object);
    }

    express_printf("vao_point destroy\n");

    g_free(vao_point);
}

void glTestIntAsyn(GLint a, GLuint b, GLfloat c, GLdouble d)
{
    printf("glTestInt asyn %d,%u,%f,%lf\n", a, b, c, d);
    fflush(stdout);
}

void glPrintfAsyn(GLint a, GLuint size, GLdouble c, const GLchar *out_string)
{

    printf("glPrintfAsyn asyn string %d,%u,%lf,%s\n", a, size, c, out_string);
    return;
}

GLint glTestInt1(GLint a, GLuint b)
{
    express_printf("glTestInt1 %d,%u\n", a, b);

    return 576634565;
}
GLuint glTestInt2(GLint a, GLuint b)
{
    printf("glTestInt2 %d,%u\n", a, b);
    fflush(stdout);
    return 4000001200u;
}

GLint64 glTestInt3(GLint64 a, GLuint64 b)
{
    printf("glTestInt3 %lld,%llu\n", a, b);
    fflush(stdout);
    return 453489431344456;
}
GLuint64 glTestInt4(GLint64 a, GLuint64 b)
{
    printf("glTestInt4 %lld,%llu\n", a, b);
    fflush(stdout);
    return 436004354364364345;
}

GLfloat glTestInt5(GLint a, GLuint b)
{
    printf("glTestInt5 %d,%u\n", a, b);
    fflush(stdout);
    return 3.1415926;
}
GLdouble glTestInt6(GLint a, GLuint b)
{
    printf("glTestInt6 %d,%u\n", a, b);
    fflush(stdout);
    return 3.1415926535;
}

void glTestPointer1(GLint a, const GLint *b)
{
    printf("glTestPointer1 %d ", a);
    for (int i = 0; i < 10; i++)
    {
        printf("%d ", b[i]);
    }
    printf("\n");
    fflush(stdout);
    return;
}

void glTestPointer2(GLint a, const GLint *b, GLint *c)
{
    printf("glTestPointer2 %d %d\n", a, *b);
    for (int i = 0; i < 10; i++)
    {
        c[i] = b[i];
    }
    fflush(stdout);
    return;
}

GLint glTestPointer4(GLint a, const GLint *b, GLint *c)
{
    printf("glTestPointer4 %d,%d\n", a, *b);
    for (int i = 0; i < 1000; i++)
    {
        c[i] = b[i];
    }
    fflush(stdout);
    return 12456687;
}

GLint d_glTestPointer3(void *context, GLint a, const GLint *b, GLint *c)
{

    int len;
    char *temp = g_malloc(a * sizeof(int));
    memset(temp, 0, a * sizeof(int));
    printf("glTestPointer3 %d\n", a);
    guest_write((Guest_Mem *)b, temp, 0, a * sizeof(int));

    char *temp_s[100];
    int loc = 0;
    for (int i = a / 2; i < a / 2 + 10; i++)
    {
        loc += sprintf(temp_s + loc, "%d ", temp[i]);
    }
    printf("glTestPointer3 %s\n", temp_s);

    guest_read((Guest_Mem *)c, temp, 0, a * sizeof(int));

    fflush(stdout);
    return 12456687;
}

void glTestString(GLint a, GLint count, const GLchar *const *strings, GLint buf_len, GLchar *char_buf)
{
    printf("glTestString %d %d %d\nString:\n", a, count, buf_len);
    for (int i = 0; i < count; i++)
    {
        printf("%s\n", strings[i]);
    }
    char *t = "printf ok!";
    memcpy(char_buf, t, strlen(t));
    fflush(stdout);
}

void d_glPrintf(void *context, GLint buf_len, const GLchar *out_string)
{

    char *temp = g_malloc(buf_len);
    guest_write((Guest_Mem *)out_string, temp, 0, buf_len);

    if (buf_len < 100)
    {
        printf("glPrintf %d %s\n", buf_len, temp);
    }
    else
    {
        int flag = 1;
        for (int i = 0; i < buf_len; i++)
        {
            if (temp[i] != 'c')
            {
                flag = 0;
            }
        }
        if (flag == 0)
        {
            printf("glPrintf check error!\n");
        }
        else
        {
            printf("glPrintf check ok!\n");
        }
    }
    g_free(temp);

    return;
}

void d_glInOutTest(void *context, GLint a, GLint b, const GLchar *e, GLint *c, GLdouble *d, GLsizei buf_len, GLchar *f)
{
}
