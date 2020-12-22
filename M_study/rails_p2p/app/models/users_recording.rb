class UsersRecording < ApplicationRecord
  belongs_to :user
  belongs_to :recording

  enum recording_status: { writing: 0, close: 1 }

  attribute :recording_status, :integer, default: :writing

  def wav_file_name
    "#{user_id}.wav"
  end
end
