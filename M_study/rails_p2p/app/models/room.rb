class Room < ApplicationRecord
  has_many :room_users, autosave: false
end
