class RoomChannel < ApplicationCable::Channel
  def subscribed
    room_user = RoomUser.find_by(room_id: params[:room_id], user_id: current_user.id)
    reject if room_user.blank?

    room_user.online!(connection.connection_identifier)

    @room = room_user.room

    stream_for broadcasting_for(@room)
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

  def recording(data)
    file_path = Rails.root.join("audio/#{@room.id}/#{current_user.id}.wav")
    FileUtils.mkdir_p(file_path.dirname)

    @writer ||= WaveFile::Writer.new(file_path.to_s, WaveFile::Format.new(:mono, :pcm_16, 44100))
    buffer = WaveFile::Buffer.new(data['buffer'].values, WaveFile::Format.new(:mono, :float, 44100))
    @writer.write(buffer)
  end

  def stop_recording
    @writer.close
  end
end
