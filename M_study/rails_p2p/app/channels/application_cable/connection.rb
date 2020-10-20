module ApplicationCable
  class Connection < ActionCable::Connection::Base
    identified_by :current_user

    def connect
      if authenticated_user.blank?
        return reject_unauthorized_connection
      end

      self.current_user = authenticated_user
    end

    private

    def authenticated_user
      request.env['warden'].user
    end
  end
end
