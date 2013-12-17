#include <cstdio>
#include <exception>
#include <set>
#include <memory>
#include <map>
#include <vector>
#include <utility>

#ifndef VIRUS_GENEALOGY_H
#define VIRUS_GENEALOGY_H

class Virus;

class VirusAlreadyCreated : std::exception {
};

class VirusNotFound : std::exception {
};

class TriedToRemoveStemVirus : std::exception {
};

template <class Virus>
class VirusGeneaology {

private:
	//VirusGeneaology(VirusGeneaology &virgen) = delete;
	//operator=(VirusGeneaology &virgen) = delete;
	typedef typename Virus::id_type v_id;
	
	v_id stem_id;

	//Klasa reprezentujaca wierzcholek, mamy set dzieci i set rodzicow
	class Node {
		public:
			v_id id;									//ID wierzcholka
			std::shared_ptr<Virus> virus;				//wskaznik na obiekt wirusa
			std::set<std::shared_ptr<Node> > chilren;   //set dzieci
			std::set<std::weak_ptr<Node> > parents;     //set rodzicow

			Node(v_id newId):
			id(newId),
			virus(new Virus(newId))
			{
			}
	};

	typedef std::pair<v_id,std::weak_ptr<Node> > map_entry;

	//Mapa wierzcholkow
	std::map<v_id, std::weak_ptr<Node> > nodes;

public:
	// Tworzy nowa genealogie.
	// Tworzy takze wezel wirusa macierzystego o identyfikatorze stem_id.
	VirusGeneaology(v_id const &new_stem_id):
	stem_id(new_stem_id)
	{
		nodes.insert(map_entry(new_stem_id,std::weak_ptr<Node> (new Node(new_stem_id))));
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
		
		std::vector<v_id> result (*(nodes.find(id)) -> children.size());
		int i = 0;
		//Przechodzimy po secie dzieci i dodajemy do wektora dzieci
		for (std::shared_ptr<Node> child : *(nodes.find(id))-> children)
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
		
		std::vector<v_id> result (*(nodes.find(id)) -> parents.size());

		//Przechodzimy po secie rodzicow i dodajemy do wektora rodzicow
		int i = 0;
		for (std::weak_ptr<Node> parent : *(nodes.find(id)) -> parents)
			result[i++] = parent -> id;
		return result;
	}


	

	// Zwraca referencje do obiektu reprezentujacego wirus o podanym
	// identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli zadany wirus nie istnieje.
	Virus& operator[](v_id const &id) const
	{
		if (!exists(id))
			throw VirusNotFound();
		return *(*(nodes.find(id)) -> virus);
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

		nodes.insert(map_entry(id, std::weak_ptr<Node>(new Node(id))));

		std::weak_ptr<Node> createdNode= *(nodes.find(id));
		std::weak_ptr<Node> parentNode= *(nodes.find(parent_id));

		//dodajemy rodzica do listy rodzicow nowego wierzcholka
		createdNode -> parents.insert(std::weak_ptr<Node>(parentNode));

		//dodajemy  dziecko do listy dzieci rodzica
		parentNode -> children.insert(std::shared_ptr<Node>(createdNode));
	}
	void create(v_id const &id, std::vector<v_id> const &parent_ids)
	{
		if (exists(id))
			throw VirusAlreadyCreated();
		for (v_id i : parent_ids)
			if (!exists(i))
				throw VirusNotFound();
		nodes.insert(map_entry(id, std::weak_ptr<Node>(new Node(id))));
		
		std::weak_ptr<Node> createdNode= *(nodes.find(id));
		for (v_id i : parent_ids)
		{
			std::weak_ptr<Node> parentNode= *(nodes.find(i));
			//dodajemy rodzica do listy rodzicow nowego wierzcholka
			createdNode -> parents.insert(std::weak_ptr<Node>(parentNode));

			//dodajemy  dziecko do listy dzieci rodzica
			parentNode -> children.insert(std::shared_ptr<Node>(createdNode));
		}
	}

	// Dodaje nowa krawedz w grafie genealogii.
	// Zglasza wyjatek VirusNotFound, jesli ktorys z podanych wirusow nie istnieje.
	void connect(v_id const &child_id, v_id const &parent_id)
	{
		if (!exists(child_id) || !exists(parent_id))
			throw VirusNotFound();

		std::weak_ptr<Node> childNode= *(nodes.find(child_id));
		std::weak_ptr<Node> parentNode= *(nodes.find(parent_id));

		//dodajemy rodzica do listy rodzicow nowego wierzcholka
		childNode -> parents.insert(std::weak_ptr<Node>(parentNode));

		//dodajemy  dziecko do listy dzieci rodzica
		parentNode -> children.insert(std::shared_ptr<Node>(childNode));

	}

	// Usuwa wirus o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli zadany wirus nie istnieje.
	// Zglasza wyjatek TriedToRemoveStemVirus przy probie usuniecia wirusa macierzystego.
	void remove(v_id const &id)
	{
		if (!exists(id))
			throw VirusNotFound();
		if (id == get_stem_id())
			throw TriedToRemoveStemVirus();
	}
};


#endif
