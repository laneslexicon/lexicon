#include <QEvent>
#include <QString>
QString getEventType(int type) {
 QString t;
if (type == QEvent::None) {
// Not an event.
  return "QEvent::None";
}
if (type == QEvent::ActionAdded) {
// A new action has been added (QActionEvent).
  return "QEvent::ActionAdded";
}
if (type == QEvent::ActionChanged) {
// An action has been changed (QActionEvent).
  return "QEvent::ActionChanged";
}
if (type == QEvent::ActionRemoved) {
// An action has been removed (QActionEvent).
  return "QEvent::ActionRemoved";
}
if (type == QEvent::ActivationChange) {
// A widget's top-level window activation state has changed.
  return "QEvent::ActivationChange";
}
if (type == QEvent::ApplicationActivate) {
// The application has been made available to the user.
  return "QEvent::ApplicationActivate";
}
if (type == QEvent::ApplicationDeactivate) {
// The application has been suspended, and is unavailable to the user.
  return "QEvent::ApplicationDeactivate";
}
if (type == QEvent::ApplicationFontChange) {
// The default application font has changed.
  return "QEvent::ApplicationFontChange";
}
if (type == QEvent::ApplicationLayoutDirectionChange) {
// The default application layout direction has changed.
  return "QEvent::ApplicationLayoutDirectionChange";
}
if (type == QEvent::ApplicationPaletteChange) {
// The default application palette has changed.
  return "QEvent::ApplicationPaletteChange";
}
if (type == QEvent::ApplicationWindowIconChange) {
// The application's icon has changed.
  return "QEvent::ApplicationWindowIconChange";
}
if (type == QEvent::ChildAdded) {
// An object gets a child (QChildEvent).
  return "QEvent::ChildAdded";
}
if (type == QEvent::ChildPolished) {
// A widget child gets polished (QChildEvent).
  return "QEvent::ChildPolished";
}
if (type == QEvent::ChildRemoved) {
// An object loses a child (QChildEvent).
  return "QEvent::ChildRemoved";
}
if (type == QEvent::Clipboard) {
// The clipboard contents have changed (QClipboardEvent).
  return "QEvent::Clipboard";
}
if (type == QEvent::Close) {
// Widget was closed (QCloseEvent).
  return "QEvent::Close";
}
if (type == QEvent::CloseSoftwareInputPanel) {
// A widget wants to close the software input panel (SIP).
  return "QEvent::CloseSoftwareInputPanel";
}
if (type == QEvent::ContentsRectChange) {
// The margins of the widget's content rect changed.
  return "QEvent::ContentsRectChange";
}
if (type == QEvent::ContextMenu) {
// Context popup menu (QContextMenuEvent).
  return "QEvent::ContextMenu";
}
if (type == QEvent::CursorChange) {
// The widget's cursor has changed.
  return "QEvent::CursorChange";
}
if (type == QEvent::DeferredDelete) {
// The object will be deleted after it has cleaned up (QDeferredDeleteEvent).
  return "QEvent::DeferredDelete";
}
if (type == QEvent::DragEnter) {
// The cursor enters a widget during a drag and drop operation (QDragEnterEvent).
  return "QEvent::DragEnter";
}
if (type == QEvent::DragLeave) {
// The cursor leaves a widget during a drag and drop operation (QDragLeaveEvent).
  return "QEvent::DragLeave";
}
if (type == QEvent::DragMove) {
// A drag and drop operation is in progress (QDragMoveEvent).
  return "QEvent::DragMove";
}
if (type == QEvent::Drop) {
// A drag and drop operation is completed (QDropEvent).
  return "QEvent::Drop";
}
if (type == QEvent::DynamicPropertyChange) {
// A dynamic property was added, changed, or removed from the object.
  return "QEvent::DynamicPropertyChange";
}
if (type == QEvent::EnabledChange) {
// Widget's enabled state has changed.
  return "QEvent::EnabledChange";
}
if (type == QEvent::Enter) {
// Mouse enters widget's boundaries (QEnterEvent).
  return "QEvent::Enter";
}
//if (type == QEvent::EnterEditFocus) {
// An editor widget gains focus for editing. QT_KEYPAD_NAVIGATION must be defined.
 // return "QEvent::EnterEditFocus";
//}
if (type == QEvent::EnterWhatsThisMode) {
// Send to toplevel widgets when the application enters "What's This?" mode.
  return "QEvent::EnterWhatsThisMode";
}
if (type == QEvent::Expose) {
// Sent to a window when its on-screen contents are invalidated and need to be flushed from the backing store.
  return "QEvent::Expose";
}
if (type == QEvent::FileOpen) {
// File open request (QFileOpenEvent).
  return "QEvent::FileOpen";
}
if (type == QEvent::FocusIn) {
// Widget or Window gains keyboard focus (QFocusEvent).
  return "QEvent::FocusIn";
}
if (type == QEvent::FocusOut) {
// Widget or Window loses keyboard focus (QFocusEvent).
  return "QEvent::FocusOut";
}
if (type == QEvent::FocusAboutToChange) {
// Widget or Window focus is about to change (QFocusEvent)
  return "QEvent::FocusAboutToChange";
}
if (type == QEvent::FontChange) {
// Widget's font has changed.
  return "QEvent::FontChange";
}
if (type == QEvent::Gesture) {
// A gesture was triggered (QGestureEvent).
  return "QEvent::Gesture";
}
if (type == QEvent::GestureOverride) {
// A gesture override was triggered (QGestureEvent).
  return "QEvent::GestureOverride";
}
if (type == QEvent::GrabKeyboard) {
// Item gains keyboard grab (QGraphicsItem only).
  return "QEvent::GrabKeyboard";
}
if (type == QEvent::GrabMouse) {
// Item gains mouse grab (QGraphicsItem only).
  return "QEvent::GrabMouse";
}
if (type == QEvent::GraphicsSceneContextMenu) {
// Context popup menu over a graphics scene (QGraphicsSceneContextMenuEvent).
  return "QEvent::GraphicsSceneContextMenu";
}
if (type == QEvent::GraphicsSceneDragEnter) {
// The cursor enters a graphics scene during a drag and drop operation (QGraphicsSceneDragDropEvent).
  return "QEvent::GraphicsSceneDragEnter";
}
if (type == QEvent::GraphicsSceneDragLeave) {
// The cursor leaves a graphics scene during a drag and drop operation (QGraphicsSceneDragDropEvent).
  return "QEvent::GraphicsSceneDragLeave";
}
if (type == QEvent::GraphicsSceneDragMove) {
// A drag and drop operation is in progress over a scene (QGraphicsSceneDragDropEvent).
  return "QEvent::GraphicsSceneDragMove";
}
if (type == QEvent::GraphicsSceneDrop) {
// A drag and drop operation is completed over a scene (QGraphicsSceneDragDropEvent).
  return "QEvent::GraphicsSceneDrop";
}
if (type == QEvent::GraphicsSceneHelp) {
// The user requests help for a graphics scene (QHelpEvent).
  return "QEvent::GraphicsSceneHelp";
}
if (type == QEvent::GraphicsSceneHoverEnter) {
// The mouse cursor enters a hover item in a graphics scene (QGraphicsSceneHoverEvent).
  return "QEvent::GraphicsSceneHoverEnter";
}
if (type == QEvent::GraphicsSceneHoverLeave) {
// The mouse cursor leaves a hover item in a graphics scene (QGraphicsSceneHoverEvent).
  return "QEvent::GraphicsSceneHoverLeave";
}
if (type == QEvent::GraphicsSceneHoverMove) {
// The mouse cursor moves inside a hover item in a graphics scene (QGraphicsSceneHoverEvent).
  return "QEvent::GraphicsSceneHoverMove";
}
if (type == QEvent::GraphicsSceneMouseDoubleClick) {
// Mouse press again (double click) in a graphics scene (QGraphicsSceneMouseEvent).
  return "QEvent::GraphicsSceneMouseDoubleClick";
}
if (type == QEvent::GraphicsSceneMouseMove) {
// Move mouse in a graphics scene (QGraphicsSceneMouseEvent).
  return "QEvent::GraphicsSceneMouseMove";
}
if (type == QEvent::GraphicsSceneMousePress) {
// Mouse press in a graphics scene (QGraphicsSceneMouseEvent).
  return "QEvent::GraphicsSceneMousePress";
}
if (type == QEvent::GraphicsSceneMouseRelease) {
// Mouse release in a graphics scene (QGraphicsSceneMouseEvent).
  return "QEvent::GraphicsSceneMouseRelease";
}
if (type == QEvent::GraphicsSceneMove) {
// Widget was moved (QGraphicsSceneMoveEvent).
  return "QEvent::GraphicsSceneMove";
}
if (type == QEvent::GraphicsSceneResize) {
// Widget was resized (QGraphicsSceneResizeEvent).
  return "QEvent::GraphicsSceneResize";
}
if (type == QEvent::GraphicsSceneWheel) {
// Mouse wheel rolled in a graphics scene (QGraphicsSceneWheelEvent).
  return "QEvent::GraphicsSceneWheel";
}
if (type == QEvent::Hide) {
// Widget was hidden (QHideEvent).
  return "QEvent::Hide";
}
if (type == QEvent::HideToParent) {
// A child widget has been hidden.
  return "QEvent::HideToParent";
}
if (type == QEvent::HoverEnter) {
// The mouse cursor enters a hover widget (QHoverEvent).
  return "QEvent::HoverEnter";
}
if (type == QEvent::HoverLeave) {
// The mouse cursor leaves a hover widget (QHoverEvent).
  return "QEvent::HoverLeave";
}
if (type == QEvent::HoverMove) {
// The mouse cursor moves inside a hover widget (QHoverEvent).
  return "QEvent::HoverMove";
}
if (type == QEvent::IconDrag) {
// The main icon of a window has been dragged away (QIconDragEvent).
  return "QEvent::IconDrag";
}
if (type == QEvent::IconTextChange) {
// Widget's icon text has been changed.
  return "QEvent::IconTextChange";
}
if (type == QEvent::InputMethod) {
// An input method is being used (QInputMethodEvent).
  return "QEvent::InputMethod";
}
if (type == QEvent::InputMethodQuery) {
// A input method query event (QInputMethodQueryEvent)
  return "QEvent::InputMethodQuery";
}
if (type == QEvent::KeyboardLayoutChange) {
// The keyboard layout has changed.
  return "QEvent::KeyboardLayoutChange";
}
if (type == QEvent::KeyPress) {
// Key press (QKeyEvent).
  return "QEvent::KeyPress";
}
if (type == QEvent::KeyRelease) {
// Key release (QKeyEvent).
  return "QEvent::KeyRelease";
}
if (type == QEvent::LanguageChange) {
// The application translation changed.
  return "QEvent::LanguageChange";
}
if (type == QEvent::LayoutDirectionChange) {
// The direction of layouts changed.
  return "QEvent::LayoutDirectionChange";
}
if (type == QEvent::LayoutRequest) {
// Widget layout needs to be redone.
  return "QEvent::LayoutRequest";
}
if (type == QEvent::Leave) {
// Mouse leaves widget's boundaries.
  return "QEvent::Leave";
}
//if (type == QEvent::LeaveEditFocus) {
// An editor widget loses focus for editing. QT_KEYPAD_NAVIGATION must be defined.
 // return "QEvent::LeaveEditFocus";
//}
if (type == QEvent::LeaveWhatsThisMode) {
// Send to toplevel widgets when the application leaves "What's This?" mode.
  return "QEvent::LeaveWhatsThisMode";
}
if (type == QEvent::LocaleChange) {
// The system locale has changed.
  return "QEvent::LocaleChange";
}
if (type == QEvent::NonClientAreaMouseButtonDblClick) {
// A mouse double click occurred outside the client area.
  return "QEvent::NonClientAreaMouseButtonDblClick";
}
if (type == QEvent::NonClientAreaMouseButtonPress) {
// A mouse button press occurred outside the client area.
  return "QEvent::NonClientAreaMouseButtonPress";
}
if (type == QEvent::NonClientAreaMouseButtonRelease) {
// A mouse button release occurred outside the client area.
  return "QEvent::NonClientAreaMouseButtonRelease";
}
if (type == QEvent::NonClientAreaMouseMove) {
// A mouse move occurred outside the client area.
  return "QEvent::NonClientAreaMouseMove";
}
if (type == QEvent::MacSizeChange) {
// The user changed his widget sizes (Mac OS X only).
  return "QEvent::MacSizeChange";
}
if (type == QEvent::MetaCall) {
// An asynchronous method invocation via QMetaObject::invokeMethod().
  return "QEvent::MetaCall";
}
if (type == QEvent::ModifiedChange) {
// Widgets modification state has been changed.
  return "QEvent::ModifiedChange";
}
if (type == QEvent::MouseButtonDblClick) {
// Mouse press again (QMouseEvent).
  return "QEvent::MouseButtonDblClick";
}
if (type == QEvent::MouseButtonPress) {
// Mouse press (QMouseEvent).
  return "QEvent::MouseButtonPress";
}
if (type == QEvent::MouseButtonRelease) {
// Mouse release (QMouseEvent).
  return "QEvent::MouseButtonRelease";
}
if (type == QEvent::MouseMove) {
// Mouse move (QMouseEvent).
  return "QEvent::MouseMove";
}
if (type == QEvent::MouseTrackingChange) {
// The mouse tracking state has changed.
  return "QEvent::MouseTrackingChange";
}
if (type == QEvent::Move) {
// Widget's position changed (QMoveEvent).
  return "QEvent::Move";
}
if (type == QEvent::OrientationChange) {
// The screens orientation has changes (QScreenOrientationChangeEvent)
  return "QEvent::OrientationChange";
}
if (type == QEvent::Paint) {
// Screen update necessary (QPaintEvent).
  return "QEvent::Paint";
}
if (type == QEvent::PaletteChange) {
// Palette of the widget changed.
  return "QEvent::PaletteChange";
}
if (type == QEvent::ParentAboutToChange) {
// The widget parent is about to change.
  return "QEvent::ParentAboutToChange";
}
if (type == QEvent::ParentChange) {
// The widget parent has changed.
  return "QEvent::ParentChange";
}
if (type == QEvent::PlatformPanel) {
// A platform specific panel has been requested.
  return "QEvent::PlatformPanel";
}
if (type == QEvent::Polish) {
// The widget is polished.
  return "QEvent::Polish";
}
if (type == QEvent::PolishRequest) {
// The widget should be polished.
  return "QEvent::PolishRequest";
}
if (type == QEvent::QueryWhatsThis) {
// The widget should accept the event if it has "What's This?" help.
  return "QEvent::QueryWhatsThis";
}
if (type == QEvent::RequestSoftwareInputPanel) {
// A widget wants to open a software input panel (SIP).
  return "QEvent::RequestSoftwareInputPanel";
}
if (type == QEvent::Resize) {
// Widget's size changed (QResizeEvent).
  return "QEvent::Resize";
}
if (type == QEvent::ScrollPrepare) {
// The object needs to fill in its geometry information (QScrollPrepareEvent).
  return "QEvent::ScrollPrepare";
}
if (type == QEvent::Scroll) {
// The object needs to scroll to the supplied position (QScrollEvent).
  return "QEvent::Scroll";
}
if (type == QEvent::Shortcut) {
// Key press in child for shortcut key handling (QShortcutEvent).
  return "QEvent::Shortcut";
}
if (type == QEvent::ShortcutOverride) {
// Key press in child, for overriding shortcut key handling (QKeyEvent).
  return "QEvent::ShortcutOverride";
}
if (type == QEvent::Show) {
// Widget was shown on screen (QShowEvent).
  return "QEvent::Show";
}
if (type == QEvent::ShowToParent) {
// A child widget has been shown.
  return "QEvent::ShowToParent";
}
if (type == QEvent::SockAct) {
// Socket activated, used to implement QSocketNotifier.
  return "QEvent::SockAct";
}
if (type == QEvent::StateMachineSignal) {
// A signal delivered to a state machine (QStateMachine::SignalEvent).
  return "QEvent::StateMachineSignal";
}
if (type == QEvent::StateMachineWrapped) {
// The event is a wrapper for, i.e., contains, another event (QStateMachine::WrappedEvent).
  return "QEvent::StateMachineWrapped";
}
if (type == QEvent::StatusTip) {
// A status tip is requested (QStatusTipEvent).
  return "QEvent::StatusTip";
}
if (type == QEvent::StyleChange) {
// Widget's style has been changed.
  return "QEvent::StyleChange";
}
if (type == QEvent::TabletMove) {
// Wacom tablet move (QTabletEvent).
  return "QEvent::TabletMove";
}
if (type == QEvent::TabletPress) {
// Wacom tablet press (QTabletEvent).
  return "QEvent::TabletPress";
}
if (type == QEvent::TabletRelease) {
// Wacom tablet release (QTabletEvent).
  return "QEvent::TabletRelease";
}
if (type == QEvent::OkRequest) {
// Ok button in decoration pressed. Supported only for Windows CE.
  return "QEvent::OkRequest";
}
if (type == QEvent::TabletEnterProximity) {
// Wacom tablet enter proximity event (QTabletEvent), sent to QApplication.
  return "QEvent::TabletEnterProximity";
}
if (type == QEvent::TabletLeaveProximity) {
// Wacom tablet leave proximity event (QTabletEvent), sent to QApplication.
  return "QEvent::TabletLeaveProximity";
}
if (type == QEvent::ThreadChange) {
// The object is moved to another thread. This is the last event sent to this object in the previous thread. See QObject::moveToThread().
  return "QEvent::ThreadChange";
}
if (type == QEvent::Timer) {
// Regular timer events (QTimerEvent).
  return "QEvent::Timer";
}
if (type == QEvent::ToolBarChange) {
// The toolbar button is toggled on Mac OS X.
  return "QEvent::ToolBarChange";
}
if (type == QEvent::ToolTip) {
// A tooltip was requested (QHelpEvent).
  return "QEvent::ToolTip";
}
if (type == QEvent::ToolTipChange) {
// The widget's tooltip has changed.
  return "QEvent::ToolTipChange";
}
if (type == QEvent::TouchBegin) {
// Beginning of a sequence of touch-screen or track-pad events (QTouchEvent).
  return "QEvent::TouchBegin";
}
if (type == QEvent::TouchCancel) {
// Cancellation of touch-event sequence (QTouchEvent).
  return "QEvent::TouchCancel";
}
if (type == QEvent::TouchEnd) {
// End of touch-event sequence (QTouchEvent).
  return "QEvent::TouchEnd";
}
if (type == QEvent::TouchUpdate) {
// Touch-screen event (QTouchEvent).
  return "QEvent::TouchUpdate";
}
if (type == QEvent::UngrabKeyboard) {
// Item loses keyboard grab (QGraphicsItem only).
  return "QEvent::UngrabKeyboard";
}
if (type == QEvent::UngrabMouse) {
// Item loses mouse grab (QGraphicsItem only).
  return "QEvent::UngrabMouse";
}
if (type == QEvent::UpdateLater) {
// The widget should be queued to be repainted at a later time.
  return "QEvent::UpdateLater";
}
if (type == QEvent::UpdateRequest) {
// The widget should be repainted.
  return "QEvent::UpdateRequest";
}
if (type == QEvent::WhatsThis) {
// The widget should reveal "What's This?" help (QHelpEvent).
  return "QEvent::WhatsThis";
}
if (type == QEvent::WhatsThisClicked) {
// A link in a widget's "What's This?" help was clicked.
  return "QEvent::WhatsThisClicked";
}
if (type == QEvent::Wheel) {
// Mouse wheel rolled (QWheelEvent).
  return "QEvent::Wheel";
}
if (type == QEvent::WinEventAct) {
// A Windows-specific activation event has occurred.
  return "QEvent::WinEventAct";
}
if (type == QEvent::WindowActivate) {
// Window was activated.
  return "QEvent::WindowActivate";
}
if (type == QEvent::WindowBlocked) {
// The window is blocked by a modal dialog.
  return "QEvent::WindowBlocked";
}
if (type == QEvent::WindowDeactivate) {
// Window was deactivated.
  return "QEvent::WindowDeactivate";
}
if (type == QEvent::WindowIconChange) {
// The window's icon has changed.
  return "QEvent::WindowIconChange";
}
if (type == QEvent::WindowStateChange) {
// The window's state (minimized, maximized or full-screen) has changed (QWindowStateChangeEvent).
  return "QEvent::WindowStateChange";
}
if (type == QEvent::WindowTitleChange) {
// The window title has changed.
  return "QEvent::WindowTitleChange";
}
if (type == QEvent::WindowUnblocked) {
// The window is unblocked after a modal dialog exited.
  return "QEvent::WindowUnblocked";
}
if (type == QEvent::WinIdChange) {
// The window system identifer for this native widget has changed.
  return "QEvent::WinIdChange";
}
if (type == QEvent::ZOrderChange) {
// The widget's z-order has changed. This event is never sent to top level windows.
  return "QEvent::ZOrderChange";
}
 return t;
}
