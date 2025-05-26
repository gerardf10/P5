

import sys
import numpy as np
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from pydub import AudioSegment


class AudioPlotter(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Audio Segments Viewer")
        self.setGeometry(100, 100, 900, 700)

        layout = QVBoxLayout()
        self.canvas = FigureCanvas(Figure(figsize=(10, 8)))
        layout.addWidget(self.canvas)
        self.setLayout(layout)

        self.plot_audio_segments()

    def plot_audio_segments(self):
        audio = AudioSegment.from_wav("vibratofm.wav")
        samples = np.array(audio.get_array_of_samples())

        if audio.channels > 1:
            samples = samples.reshape((-1, audio.channels))[:, 0]

        rate = audio.frame_rate

        segments = [
            (0, 5.00, "Señal completa con N1=5 N2=1 I=5"),
            (0.15, 0.1725, "Tramo de 25 ms"),
            (0.3, 0.31, "Tramo de 10 ms")
        ]

        fig = self.canvas.figure
        fig.clear()

        for i, (start, end, title) in enumerate(segments, start=1):
            ax = fig.add_subplot(3, 1, i)
            start_idx = int(start * rate)
            end_idx = int(end * rate)
            segment = samples[start_idx:end_idx]
            t = np.linspace(start, end, len(segment))
            ax.plot(t, segment)
            ax.set_title(title)
            ax.set_ylabel("Amplitud")
            ax.grid(True)

        fig.tight_layout()
        self.canvas.draw()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = AudioPlotter()
    window.show()
    sys.exit(app.exec_())
