class RoomChannel < ApplicationCable::Channel
  def subscribed
    room_user = RoomUser.find_by(room_id: params[:room_id], user_id: current_user.id)
    reject if room_user.blank?

    room_user.online!(connection.connection_identifier)

    @room = room_user.room

    stream_for @room
    stream_from broadcasting_for(room_user.solo_broadcasting_name)
  end

  def unsubscribed
    # Any cleanup needed when channel is unsubscribed
  end

  def receive(data)
    data = data.with_indifferent_access

    room_users = if data.key?(:to)
      @room.room_users.where(ws_connection_identifier: data[:to])
    else
      RoomUser.where(room_id: @room.id)
              .where.not(user_id: current_user.id)
    end

    room_users.each do |user|
      broadcast_to user.solo_broadcasting_name, data.merge(from: connection.connection_identifier)
    end
  end

  def start_recording
    recording = @room.start_recording
    @room.room_users.online.map do |r_user|
      recording.users_recordings.create(user: r_user.user)
    end

    broadcast_to @room, { message: 'StartedRecording' }
  end

  def end_recording
    broadcast_to @room, { message: 'EndedRecording' }
  end

  def write_audio(data)
    @recording ||= @room.now_recording
    @users_recording ||= @recording.users_recordings.find_by!(user: current_user)

    file_path = Rails.root.join("audio/#{@room.id}/#{@recording.id}/#{current_user.id}.wav")
    FileUtils.mkdir_p(file_path.dirname)

    @writer ||= WaveFile::Writer.new(file_path.to_s, WaveFile::Format.new(:mono, :pcm_16, 44100))
    buffer = WaveFile::Buffer.new(data['buffer'].values, WaveFile::Format.new(:mono, :float, 44100))
    @writer.write(buffer)
  end

  def close_audio
    @writer.close
    @users_recording.close!
    clear_recording_models
  end

  private

  def clear_recording_models
    @recording = nil
    @users_recording = nil
  end
end
