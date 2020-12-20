class CreateUsersRecordings < ActiveRecord::Migration[6.0]
  def change
    create_table :users_recordings do |t|
      t.references :user
      t.references :recording
      t.integer :recording_status

      t.timestamps
    end
  end
end
