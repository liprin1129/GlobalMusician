class Recording < ApplicationRecord
  has_many :users_recordings, dependent: :destroy
end
