#pragma once

namespace PFF {
	
	class layer;

	class layer_stack {
	
	public:
		layer_stack();
		~layer_stack();

		void push_layer(layer* layer);
		void pop_layer(layer* layer);
		void push_overlay(layer* overlay);
		void pop_overlay(layer* overlay);

		std::vector<layer*>::iterator begin() { return m_layers.begin(); }
		std::vector<layer*>::iterator end() { return m_layers.end(); }


	private:
		std::vector<layer*> m_layers;
		std::vector<layer*>::iterator m_layer_insert;

	};

}

