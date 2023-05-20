import 'package:croppy/src/src.dart';
import 'package:flutter/widgets.dart';

/// A mixin that provides resizing functionality to a
/// [BaseCroppableImageController].
mixin ResizeTransformation on BaseCroppableImageController {
  /// Returns `true` if the user is currently resizing the crop rect.
  bool get isResizing => _isResizing;
  bool _isResizing = false;

  /// Called when the user starts resizing the crop rect.
  @mustCallSuper
  void onResizeStart() {
    onTransformationStart();
    _isResizing = true;
  }

  /// Called when the user is resizing the crop rect. This will update the
  /// [BaseCroppableImageController.data] and notify listeners.
  void onResize({
    required Offset offset,
    required ResizeDirection direction,
  }) {
    final scaledOffset = offset / viewportScale;

    final rect = transformationInitialData!.cropRect;
    Rect newRect;

    switch (direction) {
      case ResizeDirection.toTop:
        newRect = Rect.fromLTRB(
          rect.left,
          rect.top - scaledOffset.dy,
          rect.right,
          rect.bottom,
        );
        break;
      case ResizeDirection.toBottom:
        newRect = Rect.fromLTRB(
          rect.left,
          rect.top,
          rect.right,
          rect.bottom - scaledOffset.dy,
        );
        break;
      case ResizeDirection.toLeft:
        newRect = Rect.fromLTRB(
          rect.left - scaledOffset.dx,
          rect.top,
          rect.right,
          rect.bottom,
        );
        break;
      case ResizeDirection.toRight:
        newRect = Rect.fromLTRB(
          rect.left,
          rect.top,
          rect.right - scaledOffset.dx,
          rect.bottom,
        );
        break;
      case ResizeDirection.toTopLeft:
        newRect = Rect.fromLTRB(
          rect.left - scaledOffset.dx,
          rect.top - scaledOffset.dy,
          rect.right,
          rect.bottom,
        );
        break;
      case ResizeDirection.toTopRight:
        newRect = Rect.fromLTRB(
          rect.left,
          rect.top - scaledOffset.dy,
          rect.right - scaledOffset.dx,
          rect.bottom,
        );
        break;
      case ResizeDirection.toBottomLeft:
        newRect = Rect.fromLTRB(
          rect.left - scaledOffset.dx,
          rect.top,
          rect.right,
          rect.bottom - scaledOffset.dy,
        );
        break;
      case ResizeDirection.toBottomRight:
        newRect = Rect.fromLTRB(
          rect.left,
          rect.top,
          rect.right - scaledOffset.dx,
          rect.bottom - scaledOffset.dy,
        );
        break;
    }

    data = transformationInitialData!.copyWith(
      cropRect: newRect,
    );

    onTransformation((offset, direction));
  }

  /// Called when the user ends resizing the crop rect.
  @mustCallSuper
  void onResizeEnd() {
    _isResizing = false;
    onTransformationEnd();
  }
}
