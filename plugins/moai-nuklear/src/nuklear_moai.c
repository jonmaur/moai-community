#include <string.h>
#include "nuklear_moai.h"

#define NK_IMPLEMENTATION




#include "nuklear.h"
#include <zl-gfx/headers.h>

#define NK_MOAI_TEXT_MAX 256
struct nk_moai_device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture null;
   
	u32 vbo, vao, ebo;
    u32 prog;
    u32 vert_shdr;
    u32 frag_shdr;
    s32 attrib_pos;
    s32 attrib_uv;
    s32 attrib_col;
    s32 uniform_tex;
    s32 uniform_proj;
	u32 font_tex;
};

static struct nk_moai {
    //GLFWwindow *win;
    //int width, height;
    //int display_width, display_height;
    struct nk_moai_device ogl;
  //  struct nk_context ctx;
 //   struct nk_font_atlas atlas;
    unsigned int text[NK_MOAI_TEXT_MAX];
    int text_len;
    float scroll;
} moai;

#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

NK_API void
nk_moai_device_create(void)
{
    s32 status;
    static const cc8 *vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const cc8 *fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    struct nk_moai_device *dev = &moai.ogl;
    nk_buffer_init_default(&dev->cmds);
    dev->prog = zglCreateProgram();
    dev->vert_shdr = zglCreateShader(ZGL_SHADER_TYPE_VERTEX);
    dev->frag_shdr = zglCreateShader(ZGL_SHADER_TYPE_FRAGMENT);
    zglShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
    zglShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
    zglCompileShader(dev->vert_shdr);
    zglCompileShader(dev->frag_shdr);
    zglGetShaderiv(dev->vert_shdr, ZGL_SHADER_INFO_COMPILE_STATUS, &status);
    //assert(status == GL_TRUE);
    zglGetShaderiv(dev->frag_shdr, ZGL_SHADER_INFO_COMPILE_STATUS, &status);
   // assert(status == GL_TRUE);
    zglAttachShader(dev->prog, dev->vert_shdr);
    zglAttachShader(dev->prog, dev->frag_shdr);




	dev->attrib_pos = 1;
	dev->attrib_uv = 2; 
	dev->attrib_col = 3; 
	zglBindAttribLocation(dev->prog, dev->attrib_pos, "Position");
	zglBindAttribLocation(dev->prog, dev->attrib_uv, "TexCoord");
	zglBindAttribLocation(dev->prog, dev->attrib_col, "Color");

    zglLinkProgram(dev->prog);
    zglGetProgramiv(dev->prog, ZGL_PROGRAM_INFO_LINK_STATUS, &status);
   // assert(status == GL_TRUE);

	dev->uniform_tex = zglGetUniformLocation(dev->prog, "Texture");
	dev->uniform_proj = zglGetUniformLocation(dev->prog, "ProjMtx");

    {
        /* buffer setup */
        u32 vs = sizeof(struct nk_draw_vertex);
        size_t vp = offsetof(struct nk_draw_vertex, position);
        size_t vt = offsetof(struct nk_draw_vertex, uv);
        size_t vc = offsetof(struct nk_draw_vertex, col);

		dev->vbo = zglCreateBuffer();
		dev->ebo = zglCreateBuffer();
		dev->vao = zglCreateVertexArray();

        zglBindVertexArray(dev->vao);
        zglBindBuffer(ZGL_BUFFER_TARGET_ARRAY, dev->vbo);
        zglBindBuffer(ZGL_BUFFER_TARGET_ELEMENT_ARRAY, dev->ebo);

        zglEnableVertexAttribArray(dev->attrib_pos);
        zglEnableVertexAttribArray(dev->attrib_uv);
        zglEnableVertexAttribArray(dev->attrib_col);

        zglVertexAttribPointer(dev->attrib_pos, 2, ZGL_TYPE_FLOAT, false, vs, (void*)vp);
        zglVertexAttribPointer(dev->attrib_uv, 2, ZGL_TYPE_FLOAT, false, vs, (void*)vt);
        zglVertexAttribPointer(dev->attrib_col, 4, ZGL_TYPE_UNSIGNED_BYTE, true, vs, (void*)vc);
    }

    zglBindTexture(0);
	zglBindBuffer(ZGL_BUFFER_TARGET_ARRAY, 0);
	zglBindBuffer(ZGL_BUFFER_TARGET_ELEMENT_ARRAY, 0);
    zglBindVertexArray(0);
}

/*
NK_INTERN void
nk_glfw3_device_upload_atlas(const void *image, int width, int height)
{
    struct nk_glfw_device *dev = &glfw.ogl;
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
}
*/
NK_API void
nk_moai_device_destroy(void)
{
    struct nk_moai_device *dev = &moai.ogl;

    zglDeleteShader(dev->vert_shdr);
    zglDeleteShader(dev->frag_shdr);
    zglDeleteProgram(dev->prog);
    //zglDeleteTextures(1, &dev->font_tex);
    zglDeleteBuffer(dev->vbo);
    zglDeleteBuffer(dev->ebo);
    nk_buffer_free(&dev->cmds);
}

NK_API void
nk_moai_render(nk_context *ctx, enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer, int width, int height)
{
    struct nk_moai_device *dev = &moai.ogl;
    //int width, height;
    float ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    //glfwGetWindowSize(glfw.win, &width, &height);
    ortho[0][0] /= (float)width;
    ortho[1][1] /= (float)height;

    /* setup global state */
    zglEnable(ZGL_PIPELINE_BLEND);
    zglBlendMode(ZGL_BLEND_MODE_ADD);
    zglBlendFunc(ZGL_BLEND_FACTOR_SRC_ALPHA, ZGL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
    zglDisable(ZGL_PIPELINE_CULL);
    zglDisable(ZGL_PIPELINE_DEPTH);
    zglEnable(ZGL_PIPELINE_SCISSOR);
    zglActiveTexture(0);

    /* setup program */
    zglUseProgram(dev->prog);
    zglUniform1i(dev->uniform_tex, 0);
    zglUniformMatrix4fv(dev->uniform_proj, 1, false, &ortho[0][0]);
    {
        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command *cmd;
        void *vertices, *elements;
        const nk_draw_index *offset = NULL;

        /* allocate vertex and element buffer */
        zglBindVertexArray(dev->vao);
		zglBindBuffer(ZGL_BUFFER_TARGET_ARRAY, dev->vbo);
		zglBindBuffer(ZGL_BUFFER_TARGET_ELEMENT_ARRAY, dev->ebo);

        zglBufferData(ZGL_BUFFER_TARGET_ARRAY, max_vertex_buffer, NULL, ZGL_BUFFER_USAGE_STREAM_DRAW);
        zglBufferData(ZGL_BUFFER_TARGET_ELEMENT_ARRAY, max_element_buffer, NULL, ZGL_BUFFER_USAGE_STREAM_DRAW);

        /* load draw vertices & elements directly into vertex + element buffer */
        vertices = zglMapBuffer(ZGL_BUFFER_TARGET_ARRAY);
        elements = zglMapBuffer(ZGL_BUFFER_TARGET_ELEMENT_ARRAY);
        {
            /* fill converting configuration */
            struct nk_convert_config config;
            memset(&config, 0, sizeof(config));
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.null = dev->null;

            /* setup buffers to load vertices and elements */
            {struct nk_buffer vbuf, ebuf;
            nk_buffer_init_fixed(&vbuf, vertices, (size_t)max_vertex_buffer);
            nk_buffer_init_fixed(&ebuf, elements, (size_t)max_element_buffer);
            nk_convert(ctx, &dev->cmds, &vbuf, &ebuf, &config);}
        }
        zglUnmapBuffer(ZGL_BUFFER_TARGET_ARRAY);
        zglUnmapBuffer(ZGL_BUFFER_TARGET_ELEMENT_ARRAY);

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, ctx, &dev->cmds) {
            if (!cmd->elem_count) continue;
            zglBindTexture(cmd->texture.id);
            zglScissor(cmd->clip_rect.x,
                height - (cmd->clip_rect.y + cmd->clip_rect.h),
                cmd->clip_rect.w, cmd->clip_rect.h);
            zglDrawElements(ZGL_PRIM_TRIANGLES, cmd->elem_count, ZGL_TYPE_UNSIGNED_SHORT, offset);
            offset += cmd->elem_count;
        }
        nk_clear(ctx);
    }

    /* default OpenGL state */
	zglBindTexture(0);
	zglBindBuffer(ZGL_BUFFER_TARGET_ARRAY, 0);
	zglBindBuffer(ZGL_BUFFER_TARGET_ELEMENT_ARRAY, 0);
	zglBindVertexArray(0);
	zglDisable(ZGL_PIPELINE_BLEND);
	zglDisable(ZGL_PIPELINE_SCISSOR);
}

/*
NK_API void
nk_glfw3_char_callback(GLFWwindow *win, unsigned int codepoint)
{
    (void)win;
    if (glfw.text_len < NK_GLFW_TEXT_MAX)
        glfw.text[glfw.text_len++] = codepoint;
}

NK_API void
nk_gflw3_scroll_callback(GLFWwindow *win, double xoff, double yoff)
{
    (void)win; (void)xoff;
    glfw.scroll += (float)yoff;
}


static void
nk_glfw3_clipbard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    const char *text = glfwGetClipboardString(glfw.win);
    if (text) nk_textedit_paste(edit, text, nk_strlen(text));
    (void)usr;
}

static void
nk_glfw3_clipbard_copy(nk_handle usr, const char *text, int len)
{
    char *str = 0;
    (void)usr;
    if (!len) return;
    str = malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    glfwSetClipboardString(glfw.win, str);
    free(str);
}
*/
NK_API void
nk_moai_init(nk_context *ctx)
{
    //nk_init_default(ctx, 0);
    nk_moai_device_create();
    
}
/*
NK_API void
nk_glfw3_font_stash_begin(struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&glfw.atlas);
    nk_font_atlas_begin(&glfw.atlas);
    *atlas = &glfw.atlas;
}

NK_API void
nk_glfw3_font_stash_end(void)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&glfw.atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    nk_glfw3_device_upload_atlas(image, w, h);
    nk_font_atlas_end(&glfw.atlas, nk_handle_id((int)glfw.ogl.font_tex), &glfw.ogl.null);
    if (glfw.atlas.default_font)
        nk_style_set_font(&glfw.ctx, &glfw.atlas.default_font->handle);
}
*/
NK_API void
nk_moai_new_frame(nk_context *ctx)
{
  //  int i;
   // double x, y;
    //struct GLFWwindow *win = glfw.win;

    //glfwGetWindowSize(win, &glfw.width, &glfw.height);
    //glfwGetFramebufferSize(win, &glfw.display_width, &glfw.display_height);
	/*
    nk_input_begin(ctx);
    
	for (i = 0; i < glfw.text_len; ++i)
        nk_input_unicode(ctx, glfw.text[i]);

    nk_input_key(ctx, NK_KEY_DEL, glfwGetKey(win, GLFW_KEY_DELETE) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_ENTER, glfwGetKey(win, GLFW_KEY_ENTER) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TAB, glfwGetKey(win, GLFW_KEY_TAB) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_BACKSPACE, glfwGetKey(win, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_UP, glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_DOWN, glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_START, glfwGetKey(win, GLFW_KEY_HOME) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_TEXT_END, glfwGetKey(win, GLFW_KEY_END) == GLFW_PRESS);
    nk_input_key(ctx, NK_KEY_SHIFT, glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS||
                                    glfwGetKey(win, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    if (glfwGetKey(win, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(win, GLFW_KEY_RIGHT_CONTROL)) {
        nk_input_key(ctx, NK_KEY_COPY, glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_PASTE, glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_CUT, glfwGetKey(win, GLFW_KEY_X) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_UNDO, glfwGetKey(win, GLFW_KEY_Z) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_REDO, glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START, glfwGetKey(win, GLFW_KEY_B) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END, glfwGetKey(win, GLFW_KEY_E) == GLFW_PRESS);
    } else {
        nk_input_key(ctx, NK_KEY_LEFT, glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_RIGHT, glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS);
        nk_input_key(ctx, NK_KEY_COPY, 0);
        nk_input_key(ctx, NK_KEY_PASTE, 0);
        nk_input_key(ctx, NK_KEY_CUT, 0);
        nk_input_key(ctx, NK_KEY_SHIFT, 0);
    }

    glfwGetCursorPos(win, &x, &y);
    x *= (double)(int)((float)glfw.display_width/(float)glfw.width);
    y *= (double)(int)((float)glfw.display_height/(float)glfw.height);
    nk_input_motion(ctx, (int)x, (int)y);

    nk_input_button(ctx, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    nk_input_button(ctx, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    nk_input_scroll(ctx, glfw.scroll);
    nk_input_end(&glfw.ctx);

    glfw.text_len = 0;
    glfw.scroll = 0;
	*/
}

NK_API
void nk_moai_shutdown(void)
{
   // nk_font_atlas_clear(&glfw.atlas);
    //nk_free(&moai.ctx); 
    nk_moai_device_destroy();
}

