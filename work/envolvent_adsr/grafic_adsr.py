import numpy as np
import matplotlib.pyplot as plt
import scipy.io.wavfile as wav

# Paràmetres comuns (modificats per cada instrument)
inici_sustain = 0.73

# Fitxers d'àudio i títols
fitxers_audio = [
    ("generico_def.wav", "Genèric"),
    ("percusivo1_def.wav", "Percussiu 1"),
    ("percusivo2_def.wav", "Percussiu 2"),
    ("plano_def.wav", "Pla")
]

# Crear la figura
plt.figure(figsize=(14, 10))

for i, (nom_fitxer, titol) in enumerate(fitxers_audio, 1):

    if i == 1:  # Genèric
        atac_inici, atac_fi = 0.5, 0.55
        deca_inici, deca_fi = 0.55, 0.65
        nivell_sustain = 2460
        release_inici, release_fi = 1.0, 1.1

    elif i == 2:  # Percussiu 1
        atac_inici, atac_fi = 0.5, 0.6
        deca_inici, deca_fi = 0.6, 0.7
        nivell_sustain = 0
        release_inici, release_fi = 0.01, 0.02

    elif i == 3:  # Percussiu 2
        atac_inici, atac_fi = 0.5, 0.8
        deca_inici, deca_fi = 0.8, 1.0
        nivell_sustain = 0
        release_inici, release_fi = 1.0, 1.1

    elif i == 4:  # Pla
        atac_inici, atac_fi = 0.5, 0.52
        deca_inici, deca_fi = 0.01, 0.02
        nivell_sustain = 12269
        release_inici, release_fi = 1.0, 1.1

    taxa_mostreig, dades = wav.read(nom_fitxer)

    # Si és estèreo, fem mitjana per passar-ho a mono
    if dades.ndim > 1:
        dades = dades.mean(axis=1)

    temps = np.arange(len(dades)) / taxa_mostreig

    # Índexs per fases ADSR
    indices_atac = (temps >= atac_inici) & (temps <= atac_fi)
    indices_deca = (temps >= deca_inici) & (temps <= deca_fi)
    index_sustain = int(inici_sustain * taxa_mostreig)
    indices_release = (temps >= release_inici) & (temps <= release_fi)

    # Subgràfic
    plt.subplot(2, 2, i)
    plt.plot(temps, dades, label='Senyal d\'àudio', alpha=0.7)
    plt.plot(temps[indices_atac], dades[indices_atac], color='red', label='Atac')
    plt.plot(temps[indices_deca], dades[indices_deca], color='blue', label='Decaiguda')
    plt.axhline(y=nivell_sustain, color='green', linestyle='--', label='Sosteniment')
    plt.plot(temps[indices_release], dades[indices_release], color='green', label='Alliberament')

    plt.title(titol)
    plt.xlabel("Temps (s)")
    plt.ylabel("Amplitud")
    plt.grid(True)
    plt.legend()

plt.tight_layout()
plt.show()