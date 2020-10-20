class RoomForm
  include ActiveModel::Model

  attr_accessor :model, :room_users

  delegate_missing_to :model

  validate :room_users_exists

  def initialize(room, current_user, attributes = nil)
    @model = room
    @current_user = current_user

    if attributes.blank?
      init_room_users
    else
      super(attributes)
    end
  end

  def save
    return false unless @model.valid?

    ApplicationRecord.transaction do
      model.save
      selected_room_users.each(&:save)
      RoomUser.find_or_initialize_by(room: model, user: @current_user).save
    end
  end

  def room_users_attributes=(attributes)
    @room_users = attributes.map do |i, attr|

      attr_selected = attr.delete(:selected)
      attr_id = attr.delete(:id)

      room_user = attr_id.blank? ? RoomUser.new(room: model) : RoomUser.find(attr_id)
      room_user.attributes = attr

      RoomUserWrap.new({
        model: room_user,
        selected: ActiveRecord::Type::Boolean.new.cast(attr_selected),
      })
    end
  end

  private

  def valid?
    valid = model.valid?
    valid = valid && selected_room_users.map(&:valid?).all?

    return valid
  end

  def init_room_users
    @room_users = selectable_room_users.map do |user|
      RoomUserWrap.new({
        model: @model.room_users.build(user: user),
        selected: false,
      })
    end
  end

  def selectable_room_users
    User.where.not(id: @current_user.id)
  end

  def selected_room_users
    room_users.select(&:selected)
  end

  def room_users_exists
    if @room_users.none?(&:selected)
      errors.add(:base, "please select at least one.")
    end
  end

  class RoomUserWrap
    include ActiveModel::Model

    attr_accessor :model, :selected

    delegate_missing_to :model

    def save
      model.save
    end
  end
end
