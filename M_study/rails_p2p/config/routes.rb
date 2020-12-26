Rails.application.routes.draw do
  # For details on the DSL available within this file, see https://guides.rubyonrails.org/routing.html
  devise_for :users, controllers: { sessions: 'users/sessions' }

  root to: "dummy#index"

  resources :rooms, only: %i[new create show] do
    get :recording_audio, :member
  end
end
