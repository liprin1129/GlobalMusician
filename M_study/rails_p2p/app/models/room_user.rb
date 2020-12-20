class RoomUser < ApplicationRecord
  belongs_to :user
  belongs_to :room

  scope :online, -> { where.not(ws_connection_identifier: nil) }

  def solo_broadcasting_name
    "room_user:#{id}"
  end

  def online!(identifier)
    update!(ws_connection_identifier: identifier)
  end
end
