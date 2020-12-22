class SynthesizeAudioJob < ApplicationJob
  queue_as :default

  def perform(recording_id)
    recording = Recording.find(recording_id)
    users_recordings = recording.users_recordings

    while users_recordings.any?(&:writing?) do
      sleep 1.seconds
      users_recordings.reload
    end

    file_names = users_recordings.map do |ur|
      recording.store_path.join(ur.wav_file_name)
    end

    # TODO: 音声合成コマンドを実行

    RoomChannel.broadcast_to recording.room, {
      message: 'ReloadRecordings',
      recordings: [
        { created_at: Time.now, file_url: '1.wav' }
      ]
    }
  end
end
