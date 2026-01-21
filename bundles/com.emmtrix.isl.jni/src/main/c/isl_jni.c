#include <jni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ISL_CONTEXT_MAGIC 0x49534c43u
#define ISL_SET_MAGIC 0x49534c53u

typedef struct {
  uint32_t magic;
  int ref_count;
} IslContext;

typedef struct {
  uint32_t magic;
  int ref_count;
  IslContext *context;
  char *text;
} IslSet;

static void throw_exception(JNIEnv *env, const char *class_name, const char *message) {
  jclass cls = (*env)->FindClass(env, class_name);
  if (cls == NULL) {
    return;
  }
  (*env)->ThrowNew(env, cls, message);
}

static void throw_invalid_argument(JNIEnv *env, const char *message) {
  throw_exception(env, "com/emmtrix/isl/core/IslInvalidArgumentException", message);
}

static void throw_native_error(JNIEnv *env, const char *message) {
  throw_exception(env, "com/emmtrix/isl/core/IslNativeException", message);
}

static IslContext *to_context(JNIEnv *env, jlong handle) {
  if (handle == 0) {
    throw_invalid_argument(env, "Context handle must not be 0");
    return NULL;
  }
  IslContext *context = (IslContext *)(intptr_t)handle;
  if (context->magic != ISL_CONTEXT_MAGIC || context->ref_count <= 0) {
    throw_invalid_argument(env, "Context handle is invalid or released");
    return NULL;
  }
  return context;
}

static IslSet *to_set(JNIEnv *env, jlong handle) {
  if (handle == 0) {
    throw_invalid_argument(env, "Set handle must not be 0");
    return NULL;
  }
  IslSet *set = (IslSet *)(intptr_t)handle;
  if (set->magic != ISL_SET_MAGIC || set->ref_count <= 0) {
    throw_invalid_argument(env, "Set handle is invalid or released");
    return NULL;
  }
  return set;
}

static void context_retain(IslContext *context) {
  context->ref_count++;
}

static void context_release(IslContext *context) {
  context->ref_count--;
  if (context->ref_count == 0) {
    context->magic = 0;
    free(context);
  }
}

static void set_release(IslSet *set) {
  set->ref_count--;
  if (set->ref_count == 0) {
    if (set->context != NULL) {
      context_release(set->context);
    }
    set->magic = 0;
    free(set->text);
    free(set);
  }
}

JNIEXPORT jlong JNICALL Java_com_emmtrix_isl_core_IslContext_nativeCreate(JNIEnv *env,
                                                                          jclass cls) {
  (void)cls;
  IslContext *context = (IslContext *)calloc(1, sizeof(IslContext));
  if (context == NULL) {
    throw_native_error(env, "Failed to allocate ISL context");
    return 0;
  }
  context->magic = ISL_CONTEXT_MAGIC;
  context->ref_count = 1;
  return (jlong)(intptr_t)context;
}

JNIEXPORT void JNICALL Java_com_emmtrix_isl_core_IslContext_nativeRelease(JNIEnv *env,
                                                                          jclass cls,
                                                                          jlong handle) {
  (void)cls;
  IslContext *context = to_context(env, handle);
  if (context == NULL) {
    return;
  }
  context_release(context);
}

JNIEXPORT jlong JNICALL Java_com_emmtrix_isl_core_IslSet_nativeReadFrom(JNIEnv *env,
                                                                        jclass cls,
                                                                        jlong contextHandle,
                                                                        jstring text) {
  (void)cls;
  IslContext *context = to_context(env, contextHandle);
  if (context == NULL) {
    return 0;
  }
  if (text == NULL) {
    throw_invalid_argument(env, "Set text must not be null");
    return 0;
  }
  const char *utf_text = (*env)->GetStringUTFChars(env, text, NULL);
  if (utf_text == NULL) {
    throw_native_error(env, "Failed to read set text");
    return 0;
  }
  if (utf_text[0] == '\0') {
    (*env)->ReleaseStringUTFChars(env, text, utf_text);
    throw_invalid_argument(env, "Set text must not be empty");
    return 0;
  }

  IslSet *set = (IslSet *)calloc(1, sizeof(IslSet));
  if (set == NULL) {
    (*env)->ReleaseStringUTFChars(env, text, utf_text);
    throw_native_error(env, "Failed to allocate ISL set");
    return 0;
  }

  set->text = strdup(utf_text);
  (*env)->ReleaseStringUTFChars(env, text, utf_text);
  if (set->text == NULL) {
    free(set);
    throw_native_error(env, "Failed to copy set text");
    return 0;
  }

  set->magic = ISL_SET_MAGIC;
  set->ref_count = 1;
  set->context = context;
  context_retain(context);

  return (jlong)(intptr_t)set;
}

JNIEXPORT void JNICALL Java_com_emmtrix_isl_core_IslSet_nativeRelease(JNIEnv *env,
                                                                      jclass cls,
                                                                      jlong handle) {
  (void)cls;
  IslSet *set = to_set(env, handle);
  if (set == NULL) {
    return;
  }
  set_release(set);
}
