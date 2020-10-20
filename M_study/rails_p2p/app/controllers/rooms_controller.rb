class RoomsController < ApplicationController
  def new
    @room_form = RoomForm.new(Room.new, current_user)
  end

  def create
    @room_form = RoomForm.new(
      Room.new,
      current_user,
      room_form_params
    )

    if @room_form.save
      redirect_to @room_form.model
    else
      render :new
    end
  end

  def show
    @room = Room.find(params[:id])
  end

  private

  def room_form_params
    params.require(:room_form)
          .permit(:id, room_users_attributes: %i[id selected user_id])
  end
end
