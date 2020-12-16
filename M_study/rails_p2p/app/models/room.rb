class Room < ApplicationRecord
  has_many :room_users, autosave: false
  has_many :recordings

  def start_recording
    recordings.create
  end

  def now_recording
    recordings.last
  end
end
