class AddColumnsToRoomUser < ActiveRecord::Migration[6.0]
  def change
    add_column :room_users, :ws_connection_identifier, :string
  end
end
