#include <jni.h>
#include <isl/ctx.h>
#include <isl/set.h>
#include <stdint.h>
#include <string.h>

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

static isl_ctx *to_context(JNIEnv *env, jlong handle) {
  if (handle == 0) {
    throw_invalid_argument(env, "Context handle must not be 0");
    return NULL;
  }
  return (isl_ctx *)(intptr_t)handle;
}

static isl_set *to_set(JNIEnv *env, jlong handle) {
  if (handle == 0) {
    throw_invalid_argument(env, "Set handle must not be 0");
    return NULL;
  }
  return (isl_set *)(intptr_t)handle;
}

static const char *isl_error_message(enum isl_error error) {
  switch (error) {
    case isl_error_none:
      return "No ISL error reported";
    case isl_error_abort:
      return "ISL aborted due to a fatal error";
    case isl_error_alloc:
      return "ISL allocation failed";
    case isl_error_unknown:
      return "ISL reported an unknown error";
    case isl_error_internal:
      return "ISL reported an internal error";
    case isl_error_invalid:
      return "ISL reported an invalid input";
    case isl_error_quota:
      return "ISL exceeded a resource quota";
    case isl_error_overflow:
      return "ISL reported an integer overflow";
    default:
      return "ISL reported an unspecified error";
  }
}

JNIEXPORT jlong JNICALL Java_com_emmtrix_isl_core_IslContext_nativeCreate(JNIEnv *env,
                                                                          jclass cls) {
  (void)cls;
  isl_ctx *context = isl_ctx_alloc();
  if (context == NULL) {
    throw_native_error(env, "Failed to allocate ISL context");
    return 0;
  }
  return (jlong)(intptr_t)context;
}

JNIEXPORT void JNICALL Java_com_emmtrix_isl_core_IslContext_nativeRelease(JNIEnv *env,
                                                                          jclass cls,
                                                                          jlong handle) {
  (void)cls;
  isl_ctx *context = to_context(env, handle);
  if (context == NULL) {
    return;
  }
  isl_ctx_free(context);
}

JNIEXPORT jlong JNICALL Java_com_emmtrix_isl_core_IslSet_nativeReadFrom(JNIEnv *env,
                                                                        jclass cls,
                                                                        jlong contextHandle,
                                                                        jstring text) {
  (void)cls;
  isl_ctx *context = to_context(env, contextHandle);
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

  isl_set *set = isl_set_read_from_str(context, utf_text);
  (*env)->ReleaseStringUTFChars(env, text, utf_text);
  if (set == NULL) {
    enum isl_error error = isl_ctx_get_error(context);
    if (error == isl_error_invalid) {
      throw_invalid_argument(env, isl_error_message(error));
    } else {
      throw_native_error(env, isl_error_message(error));
    }
    return 0;
  }

  return (jlong)(intptr_t)set;
}

JNIEXPORT void JNICALL Java_com_emmtrix_isl_core_IslSet_nativeRelease(JNIEnv *env,
                                                                      jclass cls,
                                                                      jlong handle) {
  (void)cls;
  isl_set *set = to_set(env, handle);
  if (set == NULL) {
    return;
  }
  isl_set_free(set);
}
