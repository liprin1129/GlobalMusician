class Recording < ApplicationRecord
  has_many :users_recordings, dependent: :destroy
  belongs_to :room

  def store_path
    Rails.root.join("audio/#{room_id}/#{id}/")
  end
end
