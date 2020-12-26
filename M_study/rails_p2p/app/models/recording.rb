class Recording < ApplicationRecord
  has_many :users_recordings, dependent: :destroy
  belongs_to :room

  def store_path
    Rails.root.join("audio/#{room_id}/#{id}/")
  end

  def synthesize_file_path
    store_path.join('1.wav').to_s
  end
end
