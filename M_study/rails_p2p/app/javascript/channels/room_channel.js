import consumer from "./consumer"

window.peers = {};

// init media
(function () {
  navigator.mediaDevices.getUserMedia({ video: true, audio: true }).then(stream => {
    window.localStream = stream
  })
}())

const isExistConnection = (peerIdentifier) => {
  return Object.keys(peers).includes(peerIdentifier)
}

const buildNewConnection = async (peerIdentifier, afterCandidatesMessage) => {
  const peerConnection = new RTCPeerConnection()

  window.localStream.getTracks().forEach(track => {
    peerConnection.addTrack(track, window.localStream);
  })

  peerConnection.onicecandidate = (e) => {
    if (!e.candidate) {
      connection.send({
        message: afterCandidatesMessage,
        to: peerIdentifier,
        sdp: peerConnection.localDescription
      })
    }
  }

  peerConnection.ontrack = function(e) {
    // let track = e.track
    let stream = e.streams[0]
    document.getElementById('remote-video').srcObject = stream
  }

  peers[peerIdentifier] = {
    connection: peerConnection,
  }

  return peerConnection
}

const receivedCallMe = (data) => {
  if (isExistConnection(data.from)) {
    return
  }

  buildNewConnection(data.from, 'Offer').then(peerConnection => {
    peerConnection.createOffer()
      .then((sdp) => peerConnection.setLocalDescription(sdp))
  })
}

const receivedOffer = (data) => {
  if (isExistConnection(data.from)) {
    return
  }

  buildNewConnection(data.from, 'Answer').then(peerConnection => {
    peerConnection.setRemoteDescription(new RTCSessionDescription(data.sdp))
      .then(() => peerConnection.createAnswer())
      .then((sdp) => peerConnection.setLocalDescription(sdp))
  })
}

const receivedAnswer = (data) => {
  const peerConnection = peers[data.from].connection
  peerConnection.setRemoteDescription(new RTCSessionDescription(data.sdp))
}

const receivedStartedRecording = (data) => {
  window.recording = true
  const audioContext = new window.AudioContext()

  const audioSource = audioContext.createMediaStreamSource(window.localStream)
  const audioProcessor = audioContext.createScriptProcessor(4096, 1, 1)

  audioSource.connect(audioProcessor)
  audioProcessor.connect(audioContext.destination)

  audioProcessor.onaudioprocess = (e) => {
    if (window.recording) {
      window.connection.perform("write_audio", { buffer: e.inputBuffer.getChannelData(0) })
    }
  }

  window.audioContext = audioContext
}

const receivedReloadRecordings = (data) => {
  const ul = document.querySelector('.recording-list')

  data.recordings.forEach(recording => {
    const li = document.createElement('li')
    const audio = document.createElement('audio')

    audio.setAttribute('src', recording.file_url)
    audio.setAttribute('controls', '')

    li.appendChild(document.createTextNode(recording.file_url))
    li.appendChild(audio)

    ul.insertBefore(li, null)
  })
}

const receivedEndedRecording = (data) => {
  console.log('received ended recording!')
  window.audioContext.close().then(() => {
    window.connection.perform('close_audio')
  })
}

window.onload = function () {
  window.connection = consumer.subscriptions.create({
    channel: "RoomChannel",
    room_id: document.getElementById('room-id').innerText,
  }, {
    connected() {
      // Called when the subscription is ready for use on the server
    },

    disconnected() {
      // Called when the subscription has been terminated by the server
    },

    received(data) {
      // Called when there's incoming data on the websocket for this channel
      const messageTypes = [
        'CallMe',
        'Offer',
        'Answer',
        'StartedRecording',
        'EndedRecording',
        'ReloadRecordings',
      ]

      if (messageTypes.includes(data.message)) {
        eval(`received${data.message}`).call(null, data)
      }
    }
  });

  document.getElementById('call-me').onclick = function () {
    connection.send({ message: 'CallMe' })
  }
  document.getElementById('start-recording').onclick = function (e) {
    window.connection.perform('start_recording')
  }
  document.getElementById('stop-recording').onclick = function (e) {
    window.connection.perform('end_recording')
  }
}

const appendMessageElem = (e) => {
  const data = JSON.parse(e.data)
  const el = document.createElement('p')
  const txt = document.createTextNode(data.message)

  el.appendChild(txt)
  document.getElementById('received-messages').insertBefore(el, null)
}
