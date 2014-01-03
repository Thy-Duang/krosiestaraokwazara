#include <cstdio>
#include <exception>
#include <set>
#include <memory>
#include <map>
#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/make_shared.hpp>
#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H



class Virus;

class VirusAlreadyCreated : public std::exception {
	public:
	const char* what() const noexcept {
		return "VirusAlreadyCreated";
	}
};

class VirusNotFound : public std::exception {
	public:
	const char* what() const noexcept {
		return "VirusNotFound";
	}
};

class TriedToRemoveStemVirus : public std::exception {
	public:
	const char* what() const noexcept {
		return "TriedToRemoveStemVirus";
	}
};

template <class Virus>
class VirusGenealogy {

private:
	//VirusGenealogy(VirusGenealogy &virgen) = delete;
	//operator=(VirusGenealogy &virgen) = delete;
	typedef typename Virus::id_type v_id;

	//Klasa reprezentujaca wierzcholek, mamy set dzieci i set rodzicow
	class Node {
		private:
			std::map<v_id, boost::weak_ptr<Node> > *nodes;
	public:
			v_id id;									//ID wierzcholka
			Virus *virus;								//obiekt wirusa
			std::set<boost::shared_ptr<Node> > children;   //set dzieci
			std::set<boost::weak_ptr<Node> > parents;     //set rodzicow
			boost::weak_ptr<Node> ja;
			typename std::map<v_id, boost::weak_ptr<Node> >::iterator iterator_do_mnie;
			Node(const v_id &newId, std::map<v_id, boost::weak_ptr<Node> > *extnodes):
			id(newId),
			virus(new Virus(newId))
			{
				nodes = extnodes;
			}
			~Node()
			{
				// usuwamy wskaznik do usuwanego wierzcholka z listy rodzicow u dzieci
				for (boost::shared_ptr<Node> child : children)
				{
					child -> parents.erase(ja);
				}
				children.erase(children.begin(), children.end());
				nodes->erase(nodes->find(id));
				delete virus;
			}
	};

	v_id stem_id;
	boost::shared_ptr<Node> stem_node;			//smart pointer pokazujacy na stem_node


	typedef std::pair<v_id,boost::weak_ptr<Node> > map_entry;

public:
	std::map<v_id, boost::weak_ptr<Node> > nodes; //mapa wierzcholkow (trzeba trzymac weak pointery, w readme wiecej)
	VirusGenealogy() = delete;
	// Tworzy nowa genealogie.
	// Tworzy takze wezel wirusa macierzystego o identyfikatorze stem_id.
	VirusGenealogy(v_id const &new_stem_id):
	stem_id(new_stem_id),
	stem_node(new Node(new_stem_id, &nodes))
	{
		stem_node->ja = boost::weak_ptr<Node>(stem_node);
		stem_node->iterator_do_mnie = nodes.insert(map_entry(new_stem_id,boost::weak_ptr<Node> (stem_node))).first;
	}
	~VirusGenealogy()
	{
		stem_node.reset();
	}
	// Podaje identyfikator wirusa macierzystego.
	v_id get_stem_id() const
	{
		return stem_id;
	}


	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	bool exists(v_id const &id) const
	{
		return (nodes.find(id) != nodes.end());
	}


	// Zwraca liste identyfikatorow bezposrednich nastepnikow wirusa
	// o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli dany wirus nie istnieje.
	std::vector<v_id> get_children(v_id const &id) const
	{
		if (!exists(id))
			throw VirusNotFound();

		std::vector<v_id> result ((nodes.find(id))->second.lock()-> children.size());
		int i = 0;
		//Przechodzimy po secie dzieci i dodajemy do wektora dzieci
		for (boost::shared_ptr<Node> child : (nodes.find(id))->second.lock()-> children)
			result[i++] = (child -> id);
		return result;
	}

	// Zwraca liste identyfikatorow bezposrednich poprzednikow wirusa
	// o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli dany wirus nie istnieje.
	std::vector<v_id> get_parents(v_id const &id) const
	{
		if (!exists(id))
			throw VirusNotFound();

		std::vector<v_id> result ((nodes.find(id))->second.lock() -> parents.size());

		//Przechodzimy po secie rodzicow i dodajemy do wektora rodzicow
		int i = 0;
		for (boost::weak_ptr<Node> parent : (nodes.find(id))->second.lock() -> parents) {
			result[i++] = parent.lock() -> id;
		}
		return result;
	}

	// Zwraca referencje do obiektu reprezentujacego wirus o podanym
	// identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli zadany wirus nie istnieje.
	Virus& operator[](v_id const &id) const
	{
		if (!exists(id))
			throw VirusNotFound();
		return 	*(nodes.find(id)->second.lock() -> virus);
	}

	// Tworzy wezel reprezentujacy nowy wirus o identyfikatorze id
	// powstaly z wirusow o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zglasza wyjatek VirusAlreadyCreated, jesli wirus o identyfikatorze
	// id juz istnieje.
	// Zglasza wyjatek VirusNotFound, jesli ktorys z wyspecyfikowanych
	// poprzednikow nie istnieje.
	void create(v_id const &id, v_id const &parent_id)
	{
		if (exists(id))
			throw VirusAlreadyCreated();
		if (!exists(parent_id))
			throw VirusNotFound();

		//wsadzamy do mapy wierzcholkow
		boost::shared_ptr<Node> newNode(new Node(id, &nodes));
		nodes.insert(map_entry(id, boost::weak_ptr<Node>(newNode)));


		boost::weak_ptr<Node> createdNode= nodes.find(id)->second;
		boost::weak_ptr<Node> parentNode= nodes.find(parent_id)->second;

		//dodajemy rodzica do listy rodzicow nowego wierzcholka
		createdNode.lock() -> parents.insert(boost::weak_ptr<Node>(parentNode));

		//dodajemy  dziecko do listy dzieci rodzica
		parentNode.lock() -> children.insert(boost::shared_ptr<Node>(createdNode));
	}

	void create(v_id const &id, std::vector<v_id> const &parent_ids)
	{
		if (exists(id))
			throw VirusAlreadyCreated();
		for (v_id i : parent_ids)
			if (!exists(i))
				throw VirusNotFound();

		//dodajemy do mapy wierzcholkow
		boost::shared_ptr<Node> newNode(new Node(id, &nodes));
		nodes.insert(map_entry(id, boost::weak_ptr<Node>(newNode)));

		//uaktualniamy listy rodzicow i dzieci
		boost::weak_ptr<Node> createdNode= nodes.find(id)->second;
		for (v_id i : parent_ids)
		{
			boost::weak_ptr<Node> parentNode= nodes.find(i)->second;
			//dodajemy rodzica do listy rodzicow nowego wierzcholka
			createdNode.lock() -> parents.insert(boost::weak_ptr<Node>(parentNode));

			//dodajemy  dziecko do listy dzieci rodzica
			parentNode.lock() -> children.insert(boost::shared_ptr<Node>(createdNode));
		}
	}

	// Dodaje nowa krawedz w grafie genealogii.
	// Zglasza wyjatek VirusNotFound, jesli ktorys z podanych wirusow nie istnieje.
	void connect(v_id const &child_id, v_id const &parent_id)
	{
		if (!exists(child_id) || !exists(parent_id))
			throw VirusNotFound();

		boost::weak_ptr<Node> childNode= nodes.find(child_id)->second;
		boost::weak_ptr<Node> parentNode= nodes.find(parent_id)->second;

		//dodajemy rodzica do listy rodzicow nowego wierzcholka
		childNode.lock() -> parents.insert(boost::weak_ptr<Node>(parentNode));

		//dodajemy  dziecko do listy dzieci rodzica
		parentNode.lock() -> children.insert(boost::shared_ptr<Node>(childNode));

	}

	// Usuwa wirus o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli zadany wirus nie istnieje.
	// Zglasza wyjatek TriedToRemoveStemVirus przy probie usuniecia wirusa macierzystego.
	void remove(v_id const &id)
	{
		if (!exists(id))
			throw VirusNotFound();
		if (id == stem_id)
			throw TriedToRemoveStemVirus();

		boost::weak_ptr<Node> removedNode= nodes.find(id)->second;
		//usuwamy wskaznik do usuwanego wierzcholka z listy dzieci u rodzica
		//(powiazania z dziecmi sie usuna w destruktorze)
		for (boost::weak_ptr<Node> parent : (removedNode.lock() -> parents))
		{
			parent.lock() -> children.erase(boost::shared_ptr<Node>(removedNode));
		}
	}
};

#endif
