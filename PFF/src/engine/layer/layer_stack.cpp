
#include "util/pffpch.h"

#include "layer_stack.h"
#include "layer.h"


namespace PFF {

	//
	layer_stack::layer_stack() {
	}
	
	//
	layer_stack::~layer_stack() {

		PFF_PROFILE_FUNCTION();

		for (layer* layer : m_layers)
			delete layer;

		CORE_LOG(Info, "Shutdown");
	}

	//
	void layer_stack::push_layer(layer* layer) {

		PFF_PROFILE_FUNCTION();

		m_layers.emplace(m_layers.begin() + m_layer_insert, layer);
		m_layer_insert++;

		layer->on_attach();
	}
	
	//
	void layer_stack::pop_layer(layer* layer) {

		PFF_PROFILE_FUNCTION();

		auto target = std::find(m_layers.begin(), m_layers.end(), layer);
		if (target != m_layers.end()) {

			m_layers.erase(target);
			m_layer_insert--;

			layer->on_detach();
		}
	}

	//
	void layer_stack::push_overlay(layer* overlay) {

		PFF_PROFILE_FUNCTION();

		m_layers.emplace_back(overlay);
		overlay->on_attach();
	}

	//
	void layer_stack::pop_overlay(layer* overlay) {

		PFF_PROFILE_FUNCTION();

		auto target = std::find(m_layers.begin(), m_layers.end(), overlay);
		if (target != m_layers.end()) {
		
			m_layers.erase(target);
			overlay->on_detach();
		}

	}
	void layer_stack::delete_all_layers() {

		PFF_PROFILE_FUNCTION();

		for (layer* layer : m_layers)
			delete layer;
		m_layer_insert = 0;
	}
}
