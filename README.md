PAV - P5: síntesis musical polifónica
=====================================

Obtenga su copia del repositorio de la práctica accediendo a [Práctica 5](https://github.com/albino-pav/P5) 
y pulsando sobre el botón `Fork` situado en la esquina superior derecha. A continuación, siga las
instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para crear una rama con el apellido de
los integrantes del grupo de prácticas, dar de alta al resto de integrantes como colaboradores del proyecto
y crear la copias locales del repositorio.

Como entrega deberá realizar un *pull request* con el contenido de su copia del repositorio. Recuerde que
los ficheros entregados deberán estar en condiciones de ser ejecutados con sólo ejecutar:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.sh
  make release
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A modo de memoria de la práctica, complete, en este mismo documento y usando el formato *markdown*, los
ejercicios indicados.

Ejercicios.
-----------

### Envolvente ADSR.

Tomando como modelo un instrumento sencillo (puede usar el InstrumentDumb), genere cuatro instrumentos que
permitan visualizar el funcionamiento de la curva ADSR.

* Un instrumento con una envolvente ADSR genérica, para el que se aprecie con claridad cada uno de sus
  parámetros: ataque (A), caída (D), mantenimiento (S) y liberación (R).
* Un instrumento *percusivo*, como una guitarra o un piano, en el que el sonido tenga un ataque rápido, no
  haya mantenimiemto y el sonido se apague lentamente.
  - Para un instrumento de este tipo, tenemos dos situaciones posibles:
    * El intérprete mantiene la nota *pulsada* hasta su completa extinción.
    * El intérprete da por finalizada la nota antes de su completa extinción, iniciándose una disminución
	  abrupta del sonido hasta su finalización.
  - Debera representar en esta memoria **ambos** posibles finales de la nota.
* Un instrumento *plano*, como los de cuerdas frotadas (violines y semejantes) o algunos de viento. En
  ellos, el ataque es relativamente rápido hasta alcanzar el nivel de mantenimiento (sin sobrecarga), y la
  liberación también es bastante rápida.

Para los cuatro casos, deberá incluir una gráfica en la que se visualice claramente la curva ADSR. Deberá
añadir la información necesaria para su correcta interpretación, aunque esa información puede reducirse a
colocar etiquetas y títulos adecuados en la propia gráfica (se valorará positivamente esta alternativa).

Després d'inspirar-nos en "dumb.orc", generem 4 fitxers .orc en base a les especificacions de l'enunciat: "generico.orc", "percusivo1.orc", "percusivo2.orc" i "plano.orc". Seguidament, aprofitem que ja tenim "doremi.sco" i l'apliquem als 4 instruments per constatar fàcilment els canvis, obtenint "generico.wav", "percusivo1.wav", "percusivo2.wav" i "plano.wav" després de fer synth. A continuació, generem un score (.sco) que ens ha de permetre analitzar-los més precisament (un fragment d'àudio molt més curt i precís) i generem els nous .wav ("generico_def.wav", "percusivo1_def.wav", "pervusivo2_def.wav" i "plano_def.wav"), que farem servir per les gràfiques.

![Grafiques](/Grafiques.png)


### Instrumentos Dumb y Seno.

Implemente el instrumento `Seno` tomando como modelo el `InstrumentDumb`. La señal **deberá** formarse
mediante búsqueda de los valores en una tabla.

- Incluya, a continuación, el código del fichero `seno.cpp` con los métodos de la clase Seno.

	En el nostre cas, el fitxer es diu "instrument_sin.cpp".

```cpp
#include "instrument_sin.h"
#include <math.h>
#include <iostream>

using namespace std;
using namespace upc;

InstrumentSin::InstrumentSin(const std::string &params)
: adsr(SamplingRate, params) {
   
    x.resize(BSIZE);
    bActive = false;
    
    // Create sine table of N samples
    KeyValue kv(params); // Los parametros definidos en dumb.orc
    int N;
    if (!kv.to_int("N",N)) // Si el fichero no incluia el parametro N
      N = 40;
    tbl.resize(N);
    float phase = 0;
    float stepTbl = 2 * 3.1415926 / (float) N;
    for (int i = 0; i < N; ++i) {
        tbl[i] = sin(phase);
        phase += stepTbl;
    }
    id = 0;
}



void InstrumentSin::command(long cmd, long note, long vel) {
    if (cmd == 9) {    // Note On
        bActive = true;
        adsr.start();
        // Compute step based on MIDI note
        float freq = pow(2.0, (note - 69.0) / 12.0) * 440.0;
        step = freq * 2.0 * 3.1415926 / SamplingRate;
        A = vel / 127.0;
        phaseIndex = 0;
        id = 0;
    }
    else if (cmd == 8) { // Note Off
        adsr.stop();
    }
    else if (cmd == 0) { // All notes off
        adsr.end();
    }
}
// En instrument_sin.cpp

const std::vector<float>& InstrumentSin::synthesize() {
  // If ADSR has finished, return silence and deactivate voice
  if (!adsr.active()) {
      x.assign(x.size(), 0.0f);
      bActive = false;
      return x;
  }
  else if (!bActive)
    return x;
  // If voice not active yet, return previous buffer (silence or release tail)
  // but allow ADSR to process release phase
  
  // Fill buffer with interpolated sine samples
  for (unsigned int i = 0; i < x.size(); ++i) {
      phaseIndex += step;
      while (phaseIndex>2*3.1415926){
        phaseIndex -= 2*3.1415926;
      }
      id = (int) phaseIndex / (2*3.1415926)*tbl.size();
      x[i] = A * tbl[id];
  }

  // Apply ADSR envelope to entire buffer
  adsr(x);

  return x;
}
```

  
- Explique qué método se ha seguido para asignar un valor a la señal a partir de los contenidos en la tabla,
  e incluya una gráfica en la que se vean claramente (use pelotitas en lugar de líneas) los valores de la
  tabla y los de la señal generada.

 	Es mostra un període de la senyal sinusoidal i s'emmagatzema en una taula de consulta (look-up table) amb N mostres. A continuació, aquest període registrat a la taula es pot recórrer més ràpidament o més lentament, de manera que podem generar sons més aguts o més greus segons ens convingui.
  
- Si ha implementado la síntesis por tabla almacenada en fichero externo, incluya a continuación el código
  del método `command()`.


### Efectos sonoros.

- Incluya dos gráficas en las que se vean, claramente, el efecto del trémolo y el vibrato sobre una señal
  sinusoidal. Deberá explicar detalladamente cómo se manifiestan los parámetros del efecto (frecuencia e
  índice de modulación) en la señal generada (se valorará que la explicación esté contenida en las propias
  gráficas, sin necesidad de *literatura*).

	Senyal sinusoïdal amb trémolo: fm=20 i A=0.5

	![Tremolo](/tremolo.png)

	Senyal sinusoïdal amb vibrato: fm=10 i I=0.4

	![Vibrato](/vibrato.png)

	Hem generat els fitxer .sco configurat de tal manera que els efectes s'apliquin sobre el do central (60 a Midi).
  
- Si ha generado algún efecto por su cuenta, explique en qué consiste, cómo lo ha implementado y qué
  resultado ha producido. Incluya, en el directorio `work/ejemplos`, los ficheros necesarios para apreciar
  el efecto, e indique, a continuación, la orden necesaria para generar los ficheros de audio usando el
  programa `synth`.

  Hem aplicat un clipping, és a dir, que quan superem un llindar el senyal es satura i no va més enllà. Això es veu clarament amb l'amplitud màxima del senyal (que podem localitzar al marge superior esquerre) si tolerem el 100% (clipping_out), el 80% (clipping80) o el 50% (clipping50). Observem que a mesura que anem avançant pels 3 casos anem perdent matisos del comportament del senyal en el seu tram superior.

  ![Clipping_out](/clipping_out.png)
  ![Clipping80](/clipping80.png)
  ![Clipping50](/clipping50.png)

### Síntesis FM.

Construya un instrumento de síntesis FM, según las explicaciones contenidas en el enunciado y el artículo
de [John M. Chowning](https://web.eecs.umich.edu/~fessler/course/100/misc/chowning-73-tso.pdf). El
instrumento usará como parámetros **básicos** los números `N1` y `N2`, y el índice de modulación `I`, que
deberá venir expresado en semitonos.

- Use el instrumento para generar un vibrato de *parámetros razonables* e incluya una gráfica en la que se
  vea, claramente, la correspondencia entre los valores `N1`, `N2` e `I` con la señal obtenida.
- Use el instrumento para generar un sonido tipo clarinete y otro tipo campana. Tome los parámetros del
  sonido (N1, N2 e I) y de la envolvente ADSR del citado artículo. Con estos sonidos, genere sendas escalas
  diatónicas (fichero `doremi.sco`) y ponga el resultado en los ficheros `work/doremi/clarinete.wav` y
  `work/doremi/campana.work`.
  * También puede colgar en el directorio work/doremi otras escalas usando sonidos *interesantes*. Por
    ejemplo, violines, pianos, percusiones, espadas láser de la
	[Guerra de las Galaxias](https://www.starwars.com/), etc.

### Orquestación usando el programa synth.

Use el programa `synth` para generar canciones a partir de su partitura MIDI. Como mínimo, deberá incluir la
*orquestación* de la canción *You've got a friend in me* (fichero `ToyStory_A_Friend_in_me.sco`) del genial
[Randy Newman](https://open.spotify.com/artist/3HQyFCFFfJO3KKBlUfZsyW/about).

- En este triste arreglo, la pista 1 corresponde al instrumento solista (puede ser un piano, flautas,
  violines, etc.), y la 2 al bajo (bajo eléctrico, contrabajo, tuba, etc.).
- Coloque el resultado, junto con los ficheros necesarios para generarlo, en el directorio `work/music`.
- Indique, a continuación, la orden necesaria para generar la señal (suponiendo que todos los archivos
  necesarios están en directorio indicado).

  Creem "ToyStory_A_Friend_in_me.orc" inspirant-nos en un saxo i una tuba. Com que disposem del fitxer .sco, podem generar "ToyStory_A_Friend_in_me.wav".

  Per tant, l'ordre necessària és: "~/PAV/P5/work/music$ ~/PAV/bin/synth ToyStory_A_Friend_in_me.orc ToyStory_A_Friend_in_me.sco ToyStory_A_Friend_in_me.wav -g 0.1"

También puede orquestar otros temas más complejos, como la banda sonora de *Hawaii5-0* o el villacinco de
John Lennon *Happy Xmas (War Is Over)* (fichero `The_Christmas_Song_Lennon.sco`), o cualquier otra canción
de su agrado o composición. Se valorará la riqueza instrumental, su modelado y el resultado final.
- Coloque los ficheros generados, junto a sus ficheros `score`, `instruments` y `efffects`, en el directorio
  `work/music`.
- Indique, a continuación, la orden necesaria para generar cada una de las señales usando los distintos
  ficheros.

  Hem fet la prova amb la peça Fly Me To The Moon, de Frank Sinatra, descarregada en la versió de piano (per tant, a 2 veus: una per la mà esquerra i una altra per la mà dreta). L'odre és anàloga a l'anterior: "~/PAV/P5/work/music$ ~/PAV/bin/synth flymetothemoon.orc flymetothemoon.sco flymetothemoon.wav -g 0.1".


> NOTA:
>
> No olvide escuchar el resultado generado y comprobar que no se producen ruidos extraños o distorsiones.
> Sobre todo, tenga en cuenta la salud auditiva de quien será encargado de corregir su trabajo.
