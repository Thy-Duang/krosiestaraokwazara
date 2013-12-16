#include <cstdio>
#include <exception>
#include <set>
#include <memory>

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
	VirusGeneaology(VirusGeneaology &virgen) = delete;
	operator=(VirusGeneaology &virgen) = delete;
	Virus::id_type stem_id;

	// nie wiem, co powinno byc wartoscia w drzewie, na razie tak o sobie zrobilem.
	// Zalecana zmiana, chyba na shared pointery, co?
	// Ale trzeba trzymac przodkow i dzieci dla kadzego, chyba nei ma bata.
	std::map<Virus::id_type, std::vector<Virus::id_type> > ancestors;
	std::map<Virus::id_type, std::vector<std::shared_ptr> > progeny;

public:
	// Tworzy nowa genealogie.
	// Tworzy takze wezel wirusa macierzystego o identyfikatorze stem_id.
	VirusGeneology(Virus::id_type const &stem_id);

	// Podaje identyfikator wirusa macierzystego.
	id_type get_stem_id() const;

	// Zwraca liste identyfikatorow bezposrednich nastepnikow wirusa
	// o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli dany wirus nie istnieje.
	std::vector<Virus::id_type> get_children(Virus::id_type const &id) const;

	// Zwraca liste identyfikatorow bezposrednich poprzednikow wirusa
	// o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli dany wirus nie istnieje.
	std::vector<Virus::id_type> get_parents(Virus:id_type const &id) const;

	// Sprawdza, czy wirus o podanym identyfikatorze istnieje.
	bool exists(Virus::id_type const &id) const;

	// Zwraca referencje do obiektu reprezentujacego wirus o podanym
	// identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli zadany wirus nie istnieje.
	Virus& operator[](Virus::id_type const &id) const;

	// Tworzy wezel reprezentujacy nowy wirus o identyfikatorze id
	// powstaly z wirusow o podanym identyfikatorze parent_id lub
	// podanych identyfikatorach parent_ids.
	// Zglasza wyjatek VirusAlreadyCreated, jesli wirus o identyfikatorze
	// id juz istnieje.
	// Zglasza wyjatek VirusNotFound, jesli ktorys z wyspecyfikowanych
	// poprzednikow nie istnieje.
	void create(Virus::id_type const &id, Virus::id_type const &parent_id);
	void create(Virus::id_type const &id, std::vector<Virus::id_type> const &parent_ids);

	// Dodaje nowa krawedz w grafie genealogii.
	// Zglasza wyjatek VirusNotFound, jesli ktorys z podanych wirusow nie istnieje.
	void connect(Virus::id_type const &child_id, virus::id_type const &parent_id);

	// Usuwa wirus o podanym identyfikatorze.
	// Zglasza wyjatek VirusNotFound, jesli zadany wirus nie istnieje.
	// Zglasza wyjatek TriedToRemoveStemVirus przy probie usuniecia wirusa macierzystego.
	void remove(Virus::id_type const &id);
};

template<class Virus>
bool VirusGeneaology::exists(Virus::id_type const &id) const
{
	// wystarczy sprawdzic tylko jedna mape
	return progeny.find(id) != progeny.end();
}

void VirusGeneaology::remove(Virus::id_type const &id)
{
	if (!exists(id))
		throw VirusNotFound();
	if (id_type == get_stem_id())
		throw TriedToRemoveStemVirus();
	std::vector<Virus::id_type>::iterator del = ancestors.find(id)
}

template <class Virus>
VirusGeneaology::id_type get_stem_id()
{
	return stem_id;
}

template <class Virus>
std::vector<Virus::id_type> VirusGeneaology::id_type get_children(Virus::id_type const &id)
{
	return
}

std::vector<Virus::id_type> get_parents(Virus::id_type const &id) const;
{

}

#endif
