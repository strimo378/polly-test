package com.emmtrix.isl.core;

public class IslInvalidArgumentException extends IslException {
  public IslInvalidArgumentException(String message) {
    super(message);
  }

  public IslInvalidArgumentException(String message, Throwable cause) {
    super(message, cause);
  }
}
