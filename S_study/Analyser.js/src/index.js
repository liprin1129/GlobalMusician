/*
// *********** 1. AudioWorklet example start *********** //
let audioCtx;

document.getElementById('button1').onclick = function() {
  try {
    const AudioContext = window.AudioContext || window.webkitAudioContext;
    audioCtx = new AudioContext();
    // await audioCtx.audioWorklet.addModule('test.js');
  } catch (e) {
    throw 'Could not instantiate AudioContext: ' + e.message;
  }
}

const awlAddModule = async (audioCtx) => {
  await audioCtx.audioWorklet.addModule('src/analysis.js');
  
  const oscillator = new OscillatorNode(audioCtx);
  // getAudioStream(audioCtx);
  const bypasser = new AudioWorkletNode(audioCtx, 'bypass-processor');

  oscillator.connect(bypasser);
  bypasser.connect(audioCtx.destination);

  oscillator.start();
}
// ** AudioWorklet example end ** //
*/

/*
// *********** 2. getUserMedia example start *********** //
let micStream;
let audioStream;
let audioContext;

function createAudioContext() {
  const AudioContext = window.AudioContext || window.webkitAudioContext;
  audioContext = new AudioContext();
}

function getMedia(constraints) {
  // let stream = null;
  let micStream;

  try {
    micStream = navigator.mediaDevices.getUserMedia(constraints);
    console.log("Start mic stream.");

    // micStream = audioCtx.createMediaStreamSource(stream);

    // micStream.connect(audioCtx.destination);
    
  } catch(err) {
    console("getUSerMedia error: ", err);
  } 
  return micStream;
}

async function getAudioStream(micStream) {
  try {
    // const AudioContext = window.AudioContext || window.webkitAudioContext;
    // audioCtx = new AudioContext();
    console.log("AudioContext has been created.");

    audioStream = await audioContext.createMediaStreamSource(micStream);
    audioStream.connect(audioContext.destination);
    // await audioCtx.audioWorklet.addModule('test.js');
  } catch (e) {
    throw 'Could not instantiate AudioContext: ' + e.message;
  }
}

document.getElementById('button1').onclick = async function () {
  createAudioContext();
  micStream = await getMedia({audio: true, video: false});
  getAudioStream(micStream);
}
*/


// document.getElementById('button1').onclick = getAudioContext();
// audioCtx.resunme();
// document.getElementById('button2').onclick = function() {
//   getMedia({audio: true, video: false});
//   let micStream = audioCtx.createMediaStreamSource(stream);
//   micStream.connect(audioCtx.destination);
// }
// ** getUserMedia example end ** //

// function gotStream(stream) {
//   const AudioContext = window.AudioContext || window.webkitAudioContext;
//   // debugger;
//   audioCtx = new AudioContext();
//   audioCtx.audioWorklet.addModule('src/analysis.js');

//   console.log(audioCtx);
//   audioStream = audioCtx.createMediaStreamSource(stream);
//   const bypasser = new AudioWorkletNode(audioCtx, 'bypass-processor');
  
//   console.log("gotStream");
//   audioStream.connect(bypasser)
//   // console.log(audioStream);
//   bypasser.connect(audioCtx.destination);
// }

// document.getElementById('button2').onclick = function() {
//   // navigator.mediaDevices.getUserMedia({audio:true, video: false}).then(gotStream);
//   // awlAddModule(audioCtx);
//   getMedia({audio: true, video: false});

//   console.log("Hey");
// }

// var oscillatorNode = audioCtx.createOscillator();
// var gainNode = audioCtx.createGain();
// var finish = audioCtx.destination;
// let audioCtx;

// try {
//   const AudioContext = window.AudioContext || window.webkitAudioContext;
//   audioCtx = new AudioContext();
//   // await audioCtx.audioWorklet.addModule('test.js');
// } catch (e) {
//   throw 'Could not instantiate AudioContext: ' + e.message;
// }

// const awlAddModule = async (audioCtx) => {
//   await audioCtx.audioWorklet.addModule('test.js');
  
//   const oscillator = new OscillatorNode(audioCtx);
//   const bypasser = new AudioWorkletNode(audioCtx, 'bypass-processor');

//   oscillator.connect(bypasser);
//   bypasser.connect(audioCtx.destination);

//   oscillator.start();
// }

// awlAddModule(audioCtx);
/*
// global var to load essentia instance from wasm build
// import { Plotly } from "plotly.js/dist/plotly-basic";
// import { EssentiaPlot } from "essentia.js/dist”;
// import { EssentiaPlot } from "essentia.js/dist/essentia.js-plot";

let isEssentiaInstance = false;
// global var for web audio api AudioContext
let audioCtx;
// buffer size microphone stream (bufferSize is high in order to make PitchMelodia algo to work)
let bufferSize = 128;//8192;

let createCtx = async function () {
  try {
    const AudioContext = window.AudioContext || window.webkitAudioContext;
    audioCtx = new window.AudioContext();
    await audioCtx.audioWorklet.addModule('test.js');
  } catch (e) {
    throw 'Could not instantiate AudioContext: ' + e.message;
  }

createCtx();

// global var getUserMedia mic stream
let gumStream;

// settings for plotting
let plotContainerId = "plotDiv";
// console.log(EssentiaPlot);
let plotMelodyContour = new EssentiaPlot.PlotMelodyContour(Plotly, plotContainerId)
plotMelodyContour.plotLayout.yaxis.range = [80, 4000];

console.log('start processing');
// function startMicRecordStream(audioCtx, bufferSize) {
  // cross-browser support for getUserMedia
  navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || 
                            navigator.mozGetUserMedia || navigator.msGetUserMedia;           
  window.URL = window.URL || window.webkitURL || window.mozURL || window.msURL

  if (navigator.getUserMedia) {
    console.log('Initializing audio...')
    navigator.getUserMedia({audio: true, video: false}, function(stream) {
      gumStream = stream;
      if (gumStream.active) {
        console.log('Audio context sample rate = ' + audioCtx.sampleRate);
        var mic = audioCtx.createMediaStreamSource(stream);
        // We need the buffer size that is a power of two 
        if ((bufferSize % 2) != 0 || bufferSize < 4096) {
          throw "Choose a buffer size that is a power of two and greater than 4096"
        };
        // In most platforms where the sample rate is 44.1 kHz or 48 kHz, 
        // and the default bufferSize will be 4096, giving 10-12 updates/sec.
        console.log('Buffer size = ' + bufferSize);
        if (audioCtx.state == 'suspended') {
          audioCtx.resume();
        }
        // const scriptNode = audioCtx.createScriptProcessor(bufferSize, 1, 1);
        // onprocess callback (here we can use essentia.js algos)
        // scriptNode.onaudioprocess = onProcessCallback;
        const scriptNode = new AudioWorkletNode(audioCtx, 'scriptNode');
        
        // It seems necessary to connect the stream to a sink for the pipeline to work, contrary to documentataions.
        // As a workaround, here we create a gain node with zero gain, and connect temp to the system audio output.
        const gain = audioCtx.createGain();
        gain.gain.setValueAtTime(0, audioCtx.currentTime);
        mic.connect(scriptNode);
        scriptNode.connect(gain);
        gain.connect(audioCtx.destination);

      } else {throw 'Mic stream not active';}
    }, function(message) {
      throw 'Could not access microphone - ' + message;
    });
  } else {throw 'Could not access microphone - getUserMedia not available';};
// }


// startMicRecordStream(audioCtx, bufferSize);
*/