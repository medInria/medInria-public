import sys, code
from . import qt_bindings as qt


logo = """
                            888 8888888                  d8b
                            888   888                    Y8P
                            888   888
88888b.d88b.   .d88b.   .d88888   888   88888b.  888d888 888  8888b.
888 "888 "88b d8P  Y8b d88" 888   888   888 "88b 888P"   888     "88b
888  888  888 88888888 888  888   888   888  888 888     888 .d888888
888  888  888 Y8b.     Y88b 888   888   888  888 888     888 888  888
888  888  888  "Y8888   "Y88888 8888888 888  888 888     888 "Y888888
"""


class CommandLine(qt.QLineEdit):

    def __init__(self):
        super().__init__()
        self.prompt = ""
        self.history = [""]
        self.historyIndex = 0
 
    def keyPressEvent(self, event):
        key = event.key()
        if key == qt.Key_Down or key == qt.Key_Up:
            if key == qt.Key_Down:
                self.historyIndex = min(self.historyIndex + 1, len(self.history) - 1)
            else:
                self.historyIndex = max(self.historyIndex - 1, 0)
            self.setText(self.history[self.historyIndex])
        elif key == qt.Key_Return:
            self.historyIndex = len(self.history) - 1
            line = self.line()
            self.history[self.historyIndex] = line
            if line.strip():
                self.history.append("")
                self.historyIndex += 1
        if key != qt.Key_Backspace or self.cursorPosition() > len(self.prompt):
            super().keyPressEvent(event)
        else:
            event.accept()

    def line(self):
        return super().text()[len(self.prompt):]

    def setText(self, text=""):
        super().setText(self.prompt + text)

        
class Console(qt.QWidget):

    @classmethod 
    def createInstance(cls, title, size):
        cls.instance = cls(title, size=size)
        mainWindow = qt.qApp().getProperty('MainWindow')
        mainWindow.connect('destroyed', lambda _ : cls.instance.deleteLater())
        return cls.instance

    def __init__(self, title, size):
        super().__init__()
        self._initWindow(title, size)
        self._initOutputWidget()
        self._initInputWidget()
        self._initFonts()
        self._initPrompt()
 
    def _initWindow(self, title, size):
        self.setWindowTitle(title)
        layout = qt.QVBoxLayout()
        layout.setSpacing(0)
        self.setLayout(layout)
        self.resize(*size)

    def _initOutputWidget(self):
        self.outputWidget = qt.QLabel()
        self.outputWidget.setAlignment(qt.AlignLeft | qt.AlignBottom)
        self.outputWidget.setWordWrap(True)
        self.scrollArea = qt.QScrollArea()
        self.scrollArea.setWidget(self.outputWidget)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.horizontalScrollBar().setEnabled(False)
        verticalScrollBar = self.scrollArea.verticalScrollBar()
        verticalScrollBar.connect('rangeChanged', lambda _, max : verticalScrollBar.setValue(max))
        self.layout().addWidget(self.scrollArea)

    def _initInputWidget(self):
        self.inputWidget = CommandLine()
        self.layout().addWidget(self.inputWidget)
        self.setFocusProxy(self.inputWidget)
        self.inputWidget.connect('returnPressed', self.handleInput)
        self.inputWidget.connect('cursorPositionChanged', self.ensureCursorAfterPrompt)

    def _initFonts(self):
        font = self.outputWidget.font()
        font.setFamily("Courier")
        self.outputWidget.setFont(font)
        self.inputWidget.setFont(font)
        self.inputWidget.setFrame(False)

    def _initPrompt(self):
        try:
            self.ps1 = sys.ps1
        except AttributeError:
            self.ps1 = ">>> "
        try:
            self.ps2 = sys.ps2
        except AttributeError:
            self.ps2 = "... "
        self.inputWidget.prompt = self.ps1
        self.inputWidget.setText()

    def setShortcut(self, keySequence):
        if not hasattr(self, 'shortcut'):
            mainWindow = qt.qApp().getProperty('MainWindow')
            self.shortcut = qt.QShortcut(mainWindow)
            self.shortcut.setContext(qt.ApplicationShortcut)
            self.shortcut.connect('activated', lambda : self.setVisible(not self.isVisible()))
        self.shortcut.setKey(qt.QKeySequence(keySequence))

    def run(self):
        self.interpreter = code.InteractiveInterpreter(sys.modules['__main__'].__dict__)
        self.resetInputBuffer()
        sys.stdout = self
        sys.stderr = self
        self.printWelcomeText()

    def printWelcomeText(self):
        print(f"Python {sys.version} on {sys.platform}")
        print(logo)
        print('Type "help", "copyright", "credits" or "license" for more information.\n\n')
  
    def resetInputBuffer(self):
        self.inputBuffer = []

    def handleInput(self):
        print(self.inputWidget.text())
        line = self.inputWidget.line()
        self.inputBuffer.append(line)
        source = "\n".join(self.inputBuffer)
        try:
            more = self.interpreter.runsource(source)
            self.updateInputStatus(newCommand = not(more and line))
        except SystemExit:
            self.updateInputStatus()
            qt.qApp().getProperty('MainWindow').close()

    def updateInputStatus(self, newCommand=True):
        if newCommand:
            self.inputWidget.prompt = self.ps1
            self.resetInputBuffer()
        else:
            self.inputWidget.prompt = self.ps2
        self.inputWidget.setText()

    def ensureCursorAfterPrompt(self, oldPosition, newPosition):
        self.inputWidget.setCursorPosition(max(len(self.inputWidget.prompt), newPosition))

    def write(self, text):
        self.outputWidget.setText(f'{self.outputWidget.text()}{text}')

    def flush(self):
        pass

    def showEvent(self, event):
        self.setFocus()
