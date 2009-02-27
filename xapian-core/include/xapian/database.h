/** \file database.h
 * \brief API for working with Xapian databases
 */
/* Copyright 1999,2000,2001 BrightStation PLC
 * Copyright 2002 Ananova Ltd
 * Copyright 2002,2003,2004,2005,2006,2007,2008,2009 Olly Betts
 * Copyright 2006,2008 Lemur Consulting Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#ifndef XAPIAN_INCLUDED_DATABASE_H
#define XAPIAN_INCLUDED_DATABASE_H

#include <string>
#include <vector>

#include <xapian/base.h>
#include <xapian/document.h>
#include <xapian/types.h>
#include <xapian/positioniterator.h>
#include <xapian/postingiterator.h>
#include <xapian/termiterator.h>
#include <xapian/valueiterator.h>
#include <xapian/visibility.h>

/// The Xapian library lives in the Xapian namespace.
namespace Xapian {

/** This class is used to access a database, or a group of databases.
 *
 *  For searching, this class is used in conjunction with an Enquire object.
 *
 *  @exception InvalidArgumentError will be thrown if an invalid
 *  argument is supplied, for example, an unknown database type.
 *
 *  @exception DatabaseOpeningError may be thrown if the database cannot
 *  be opened (for example, a required file cannot be found).
 *
 *  @exception DatabaseVersionError may be thrown if the database is in an
 *  unsupported format (for example, created by a newer version of Xapian
 *  which uses an incompatible format).
 */
class XAPIAN_VISIBILITY_DEFAULT Database {
    public:
	class Internal;
	/// @private @internal Reference counted internals.
	std::vector<Xapian::Internal::RefCntPtr<Internal> > internal;

	/** @private @internal Get a document from the database, but doesn't
	 *  need to check if it exists.
	 *
	 *  This method returns a Xapian::Document object which provides the
	 *  information about a document.  If the document doesn't exist,
	 *  either a NULL pointer may be returned, or the returned object will
	 *  throw DocNotFoundError when you try to access it.
	 *
	 *  The caller should delete the returned object when it has finished
	 *  with it.
	 *
	 *  @param did   The document id of the document to retrieve.
	 *
	 *  @return      Pointer to Document::Internal object.
	 */
	Document::Internal * get_document_lazily(Xapian::docid did) const;

	/** Add an existing database (or group of databases) to those
	 *  accessed by this object.
	 *
	 *  @param database the database(s) to add.
	 */
	void add_database(const Database & database);

	/** Create a Database with no databases in.
	 */
	Database();

	/** Open a Database, automatically determining the database
	 *  backend to use.
	 *
	 * @param path directory that the database is stored in.
	 */
	explicit Database(const std::string &path);

	/** @private @internal Create a Database from its internals.
	 */
	explicit Database(Internal *internal);

	/** Destroy this handle on the database.
	 *
	 *  If there are no copies of this object remaining, the database(s)
	 *  will be closed.
	 */
	virtual ~Database();

        /** Copying is allowed.  The internals are reference counted, so
	 *  copying is cheap.
	 */
	Database(const Database &other);

        /** Assignment is allowed.  The internals are reference counted,
	 *  so assignment is cheap.
	 */
	void operator=(const Database &other);

	/** Re-open the database.
	 *  This re-opens the database(s) to the latest available version(s).
	 *  It can be used either to make sure the latest results are
	 *  returned, or to recover from a Xapian::DatabaseModifiedError.
	 */
	void reopen();

	/** Close the database.
	 *
	 *  This closes the database and releases all file handles held by the
	 *  database.
	 *
	 *  After this call, no calls should be made to other methods of the
	 *  database, or to objects derived from the database (other than
	 *  destructors).  If any such methods are called, their behaviour is
	 *  undefined: they will often raise a Xapian::DatabaseError indicating
	 *  that the database is closed, but this is not guaranteed.  The
	 *  behaviour of methods in this situation should not be relied on,
	 *  since it may change in later releases due to changes in the
	 *  implementation of the database backend.
	 *
	 *  This is a permanent close of the database: calling reopen() after
	 *  closing a database will not reopen it, and may raise an exception
	 *  just like any other method.
	 *
	 *  Calling close() on a database which is already closed has no effect
	 *  (and doesn't raise an exception).
	 */
	virtual void close();

	/// Return a string describing this object.
	virtual std::string get_description() const;

	/** An iterator pointing to the start of the postlist
	 *  for a given term.
	 *
	 *  If the term name is the empty string, the iterator returned
	 *  will list all the documents in the database.  Such an iterator
	 *  will always return a WDF value of 1, since there is no obvious
	 *  meaning for this quantity in this case.
	 */
	PostingIterator postlist_begin(const std::string &tname) const;

	/** Corresponding end iterator to postlist_begin().
	 */
	PostingIterator postlist_end(const std::string &) const {
	    return PostingIterator(NULL);
	}

	/** An iterator pointing to the start of the termlist
	 *  for a given document.
	 */
	TermIterator termlist_begin(Xapian::docid did) const;

	/** Corresponding end iterator to termlist_begin().
	 */
	TermIterator termlist_end(Xapian::docid) const {
	    return TermIterator(NULL);
	}

	/** Does this database have any positional information? */
	bool has_positions() const;

	/** An iterator pointing to the start of the position list
	 *  for a given term in a given document.
	 */
	PositionIterator positionlist_begin(Xapian::docid did, const std::string &tname) const;

	/** Corresponding end iterator to positionlist_begin().
	 */
	PositionIterator positionlist_end(Xapian::docid, const std::string &) const {
	    return PositionIterator(NULL);
	}

	/** An iterator which runs across all terms in the database.
	 */
	TermIterator allterms_begin() const;

	/** Corresponding end iterator to allterms_begin().
	 */
	TermIterator allterms_end() const {
	    return TermIterator(NULL);
	}

	/** An iterator which runs across all terms with a given prefix.
	 *
	 *  This is functionally similar to getting an iterator with
	 *  allterms_begin() and then calling skip_to(prefix) on that iterator
	 *  to move to the start of the prefix, but is more convenient (because
	 *  it detects the end of the prefixed terms), and may be more
	 *  efficient than simply calling skip_to() after opening the iterator,
	 *  particularly for network databases.
	 *
	 *  @param prefix The prefix to restrict the returned terms to.
	 */
	TermIterator allterms_begin(const std::string & prefix) const;

	/** Corresponding end iterator to allterms_begin(prefix).
	 */
	TermIterator allterms_end(const std::string &) const {
	    return TermIterator(NULL);
	}

	/// Get the number of documents in the database.
	Xapian::doccount get_doccount() const;

	/// Get the highest document id which has been used in the database.
	Xapian::docid get_lastdocid() const;

	/// Get the average length of the documents in the database.
	Xapian::doclength get_avlength() const;

	/// Get the number of documents in the database indexed by a given term.
	Xapian::doccount get_termfreq(const std::string & tname) const;

	/** Check if a given term exists in the database.
	 *
	 *  Return true if and only if the term exists in the database.
	 *  This is the same as (get_termfreq(tname) != 0), but will often be
	 *  more efficient.
	 */
	bool term_exists(const std::string & tname) const;

	/** Return the total number of occurrences of the given term.
	 *
	 *  This is the sum of the number of occurrences of the term in each
	 *  document it indexes: i.e., the sum of the within document
	 *  frequencies of the term.
	 *
	 *  @param tname  The term whose collection frequency is being
	 *  requested.
	 */
	Xapian::termcount get_collection_freq(const std::string & tname) const;

	/** Return the frequency of a given value slot.
	 *
	 *  This is the number of documents which have a (non-empty) value
	 *  stored in the slot.
	 *
	 *  @param valno The value slot to examine.
	 *
	 *  @exception UnimplementedError The frequency of the value isn't
	 *  available for this database type.
	 */
	Xapian::doccount get_value_freq(Xapian::valueno valno) const;

	/** Get a lower bound on the values stored in the given value slot.
	 *
	 *  If there are no values stored in the given value slot, this will
	 *  return an empty string.
	 *
	 *  If the lower bound isn't available for the given database type,
	 *  this will return the lowest possible bound - the empty string.
	 *
	 *  @param valno The value slot to examine.
	 */
	std::string get_value_lower_bound(Xapian::valueno valno) const;

	/** Get an upper bound on the values stored in the given value slot.
	 *
	 *  If there are no values stored in the given value slot, this will
	 *  return an empty string.
	 *
	 *  @param valno The value slot to examine.
	 *
	 *  @exception UnimplementedError The upper bound of the values isn't
	 *  available for this database type.
	 */
	std::string get_value_upper_bound(Xapian::valueno valno) const;

	/// Return an iterator over the value in slot @a slot for each document.
	ValueIterator valuestream_begin(Xapian::valueno slot) const;

	/// Return end iterator corresponding to valuestream_begin().
	ValueIterator valuestream_end(Xapian::valueno) const {
	    return ValueIterator();
	}

	/** Get the length of a document.
	 */
	Xapian::doclength get_doclength(Xapian::docid did) const;

	/** Send a "keep-alive" to remote databases to stop them timing
	 *  out.
	 */
	void keep_alive();

	/** Get a document from the database, given its document id.
	 *
	 *  This method returns a Xapian::Document object which provides the
	 *  information about a document.
	 *
	 *  @param did   The document id of the document to retrieve.
	 *
	 *  @return      A Xapian::Document object containing the document data
	 *
	 *  @exception Xapian::DocNotFoundError      The document specified
	 *		could not be found in the database.
	 */
	Xapian::Document get_document(Xapian::docid did) const;

	/** Suggest a spelling correction.
	 *
	 *  @param word			The potentially misspelled word.
	 *  @param max_edit_distance	Only consider words which are at most
	 *	@a max_edit_distance edits from @a word.  An edit is a
	 *	character insertion, deletion, or the transposition of two
	 *	adjacent characters (default is 2).
	 */
	std::string get_spelling_suggestion(const std::string &word,
					    unsigned max_edit_distance = 2) const;

	/** An iterator which returns all the spelling correction targets.
	 *
	 *  This returns all the words which are considered as targets for the
	 *  spelling correction algorithm.  The frequency of each word is
	 *  available as the term frequency of each entry in the returned
	 *  iterator.
	 */
	Xapian::TermIterator spellings_begin() const;

	/// Corresponding end iterator to spellings_begin().
	Xapian::TermIterator spellings_end() const {
	    return Xapian::TermIterator(NULL);
	}

	/** An iterator which returns all the synonyms for a given term.
	 *
	 *  @param term	    The term to return synonyms for.
	 */
	Xapian::TermIterator synonyms_begin(const std::string &term) const;

	/// Corresponding end iterator to synonyms_begin(term).
	Xapian::TermIterator synonyms_end(const std::string &) const {
	    return Xapian::TermIterator(NULL);
	}

	/** An iterator which returns all terms which have synonyms.
	 *
	 *  @param prefix   If non-empty, only terms with this prefix are
	 *		    returned.
	 */
	Xapian::TermIterator synonym_keys_begin(const std::string &prefix = std::string()) const;

	/// Corresponding end iterator to synonym_keys_begin(prefix).
	Xapian::TermIterator synonym_keys_end(const std::string & = std::string()) const {
	    return Xapian::TermIterator(NULL);
	}

	/** Get the user-specified metadata associated with a given key.
	 *
	 *  User-specified metadata allows you to store arbitrary information
	 *  in the form of (key,tag) pairs.  See @a
	 *  WritableDatabase::set_metadata() for more information.
	 *
	 *  When invoked on a Xapian::Database object representing multiple
	 *  databases, currently only the metadata for the first is considered
	 *  but this behaviour may change in the future.
	 *
	 *  If there is no piece of metadata associated with the specified
	 *  key, an empty string is returned (this applies even for backends
	 *  which don't support metadata).
	 *
	 *  Empty keys are not valid, and specifying one will cause an
	 *  exception.
	 *
	 *  @param key The key of the metadata item to access.
	 *
	 *  @return    The retrieved metadata item's value.
	 *
	 *  @exception Xapian::InvalidArgumentError will be thrown if the
	 *	       key supplied is empty.
	 *
	 *  @exception Xapian::UnimplementedError will be thrown if the
	 *	       database backend in use doesn't support user-specified
	 *	       metadata.
	 */
	std::string get_metadata(const std::string & key) const;

	/** An iterator which returns all user-specified metadata keys.
	 *
	 *  When invoked on a Xapian::Database object representing multiple
	 *  databases, currently only the metadata for the first is considered
	 *  but this behaviour may change in the future.
	 *
	 *  @param prefix   If non-empty, only keys with this prefix are
	 *		    returned.
	 */
	Xapian::TermIterator metadata_keys_begin(const std::string &prefix = std::string()) const;

	/// Corresponding end iterator to metadata_keys_begin().
	Xapian::TermIterator metadata_keys_end(const std::string & = std::string()) const {
	    return Xapian::TermIterator(NULL);
	}

	/** Get a UUID for the database.
	 *
	 *  The UUID will persist for the lifetime of the database.
	 *
	 *  Replicas (eg, made with the replication protocol, or by copying all
	 *  the database files) will have the same UUID.  However, copies (made
	 *  with copydatabase, or xapian-compact) will have different UUIDs.
	 *
	 *  If the backend does not support UUIDs, or this database has
	 *  multiple sub-databases, an exception will be raised.
	 */
	std::string get_uuid() const;
};

/** This class provides read/write access to a database.
 */
class XAPIAN_VISIBILITY_DEFAULT WritableDatabase : public Database {
    public:
	/** Destroy this handle on the database.
	 *
	 *  If there are no copies of this object remaining, the database
	 *  will be closed.  If there are any transactions in progress
	 *  these will be aborted as if cancel_transaction had been called.
	 */
	virtual ~WritableDatabase();

	/** Create an empty WritableDatabase.
	 */
	WritableDatabase();

	/** Open a database for update, automatically determining the database
	 *  backend to use.
	 *
	 *  If the database is to be created, Xapian will try
	 *  to create the directory indicated by path if it doesn't already
	 *  exist (but only the leaf directory, not recursively).
	 *
	 * @param path directory that the database is stored in.
	 * @param action one of:
	 *  - Xapian::DB_CREATE_OR_OPEN open for read/write; create if no db
	 *    exists
	 *  - Xapian::DB_CREATE create new database; fail if db exists
	 *  - Xapian::DB_CREATE_OR_OVERWRITE overwrite existing db; create if
	 *    none exists
	 *  - Xapian::DB_OPEN open for read/write; fail if no db exists
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 *
	 *  @exception Xapian::DatabaseLockError will be thrown if a lock
	 *	       couldn't be acquired on the database.
	 */
	WritableDatabase(const std::string &path, int action);

	/** @private @internal Create an WritableDatabase given its internals.
	 */
	explicit WritableDatabase(Database::Internal *internal);

        /** Copying is allowed.  The internals are reference counted, so
	 *  copying is cheap.
	 */
	WritableDatabase(const WritableDatabase &other);

        /** Assignment is allowed.  The internals are reference counted,
	 *  so assignment is cheap.
	 *
	 *  Note that only an WritableDatabase may be assigned to an
	 *  WritableDatabase: an attempt to assign a Database is caught
	 *  at compile-time.
	 */
	void operator=(const WritableDatabase &other);

	/** Flush to disk any modifications made to the database.
	 *
	 *  For efficiency reasons, when performing multiple updates to a
	 *  database it is best (indeed, almost essential) to make as many
	 *  modifications as memory will permit in a single pass through
	 *  the database.  To ensure this, Xapian batches up modifications.
	 *
	 *  Flush may be called at any time to
	 *  ensure that the modifications which have been made are written to
	 *  disk: if the flush succeeds, all the preceding modifications will
	 *  have been written to disk.
	 *
	 *  If any of the modifications fail, an exception will be thrown and
	 *  the database will be left in a state in which each separate
	 *  addition, replacement or deletion operation has either been fully
	 *  performed or not performed at all: it is then up to the
	 *  application to work out which operations need to be repeated.
	 *
	 *  It's not valid to call flush within a transaction.
	 *
	 *  Beware of calling flush too frequently: this will have a severe
	 *  performance cost.
	 *
	 *  Note that flush need not be called explicitly: it will be called
	 *  automatically when the database is closed, or when a sufficient
	 *  number of modifications have been made.  By default, this is every
	 *  10000 documents added, deleted, or modified.  This value is rather
	 *  conservative, and if you have a machine with plenty of memory,
	 *  you can improve indexing throughput dramatically by setting
	 *  XAPIAN_FLUSH_THRESHOLD in the environment to a larger value.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while modifying the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 */
	void flush();

	/** Begin a transaction.
	 *
	 *  In Xapian a transaction is a group of modifications to the database
	 *  which are linked such that either all will be applied
	 *  simultaneously or none will be applied at all.  Even in the case of
	 *  a power failure, this characteristic should be preserved (as long
	 *  as the filesystem isn't corrupted, etc).
	 *
	 *  A transaction is started with begin_transaction() and can
	 *  either be committed by calling commit_transaction() or aborted
	 *  by calling cancel_transaction().
	 *
	 *  By default, a transaction implicitly calls flush before and after
	 *  so that the modifications stand and fall without affecting
	 *  modifications before or after.
	 *
	 *  The downside of this flushing is that small transactions cause
	 *  modifications to be frequently flushed which can harm indexing
	 *  performance in the same way that explicitly calling flush
	 *  frequently can.
	 *
	 *  If you're applying atomic groups of changes and only wish to
	 *  ensure that each group is either applied or not applied, then
	 *  you can prevent the automatic flush before and after the
	 *  transaction by starting the transaction with
	 *  begin_transaction(false).  However, if cancel_transaction is
	 *  called (or if commit_transaction isn't called before the
	 *  WritableDatabase object is destroyed) then any changes which
	 *  were pending before the transaction began will also be discarded.
	 *
	 *  Transactions aren't currently supported by the InMemory backend.
	 *
	 *  @exception Xapian::UnimplementedError will be thrown if transactions
	 *             are not available for this database type.
	 *
	 *  @exception Xapian::InvalidOperationError will be thrown if this is
	 *             called at an invalid time, such as when a transaction
	 *             is already in progress.
	 */
	void begin_transaction(bool flushed=true);

	/** Complete the transaction currently in progress.
	 *
	 *  If this method completes successfully and this is a flushed
	 *  transaction, all the database modifications
	 *  made during the transaction will have been committed to the
	 *  database.
	 *
	 *  If an error occurs, an exception will be thrown, and none of
	 *  the modifications made to the database during the transaction
	 *  will have been applied to the database.
	 *
	 *  In all cases the transaction will no longer be in progress.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while modifying the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 *
	 *  @exception Xapian::InvalidOperationError will be thrown if a
	 *	       transaction is not currently in progress.
	 *
	 *  @exception Xapian::UnimplementedError will be thrown if transactions
	 *             are not available for this database type.
	 */
	void commit_transaction();

	/** Abort the transaction currently in progress, discarding the
	 *  potential modifications made to the database.
	 *
	 *  If an error occurs in this method, an exception will be thrown,
	 *  but the transaction will be cancelled anyway.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while modifying the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 *
	 *  @exception Xapian::InvalidOperationError will be thrown if a
	 *	       transaction is not currently in progress.
	 *
	 *  @exception Xapian::UnimplementedError will be thrown if transactions
	 *             are not available for this database type.
	 */
	void cancel_transaction();

	/** Add a new document to the database.
	 *
	 *  This method adds the specified document to the database,
	 *  returning a newly allocated document ID.  Automatically allocated
	 *  document IDs come from a per-database monotonically increasing
	 *  counter, so IDs from deleted documents won't be reused.
	 *
	 *  If you want to specify the document ID to be used, you should
	 *  call replace_document() instead.
	 *
	 *  Note that changes to the database won't be immediately committed to
	 *  disk; see flush() for more details.
	 *
	 *  As with all database modification operations, the effect is
	 *  atomic: the document will either be fully added, or the document
	 *  fails to be added and an exception is thrown (possibly at a
	 *  later time when flush is called or the database is closed).
	 *
	 *  @param document The new document to be added.
	 *
	 *  @return         The document ID of the newly added document.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while writing to the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 */
	Xapian::docid add_document(const Xapian::Document & document);

	/** Delete a document from the database.
	 *
	 *  This method removes the document with the specified document ID
	 *  from the database.
	 *
	 *  Note that changes to the database won't be immediately committed to
	 *  disk; see flush() for more details.
	 *
	 *  As with all database modification operations, the effect is
	 *  atomic: the document will either be fully removed, or the document
	 *  fails to be removed and an exception is thrown (possibly at a
	 *  later time when flush is called or the database is closed).
	 *
	 *  @param did     The document ID of the document to be removed.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while writing to the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 */
	void delete_document(Xapian::docid did);

	/** Delete any documents indexed by a term from the database.
	 *
	 *  This method removes any documents indexed by the specified term
	 *  from the database.
	 *
	 *  A major use is for convenience when UIDs from another system are
	 *  mapped to terms in Xapian, although this method has other uses
	 *  (for example, you could add a "deletion date" term to documents at
	 *  index time and use this method to delete all documents due for
	 *  deletion on a particular date).
	 *
	 *  @param unique_term     The term to remove references to.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while writing to the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 */
	void delete_document(const std::string & unique_term);

	/** Replace a given document in the database.
	 *
	 *  This method replaces the document with the specified document ID.
	 *  If document ID @a did isn't currently used, the document will be
	 *  added with document ID @a did.
	 *
	 *  The monotonic counter used for automatically allocating document
	 *  IDs is increased so that the next automatically allocated document
	 *  ID will be did + 1.  Be aware that if you use this method to
	 *  specify a high document ID for a new document, and also use
	 *  WritableDatabase::add_document(), Xapian may get to a state where
	 *  this counter wraps around and will be unable to automatically
	 *  allocate document IDs!
	 *
	 *  Note that changes to the database won't be immediately committed to
	 *  disk; see flush() for more details.
	 *
	 *  As with all database modification operations, the effect is
	 *  atomic: the document will either be fully replaced, or the document
	 *  fails to be replaced and an exception is thrown (possibly at a
	 *  later time when flush is called or the database is closed).
	 *
	 *  @param did     The document ID of the document to be replaced.
	 *  @param document The new document.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while writing to the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 */
	void replace_document(Xapian::docid did,
			      const Xapian::Document & document);

	/** Replace any documents matching a term.
	 *
	 *  This method replaces any documents indexed by the specified term
	 *  with the specified document.  If any documents are indexed by the
	 *  term, the lowest document ID will be used for the document,
	 *  otherwise a new document ID will be generated as for add_document.
	 *
	 *  The intended use is to allow UIDs from another system to easily
	 *  be mapped to terms in Xapian, although this method probably has
	 *  other uses.
	 *
	 *  Note that changes to the database won't be immediately committed to
	 *  disk; see flush() for more details.
	 *
	 *  As with all database modification operations, the effect is
	 *  atomic: the document(s) will either be fully replaced, or the
	 *  document(s) fail to be replaced and an exception is thrown
	 *  (possibly at a
	 *  later time when flush is called or the database is closed).
	 *
	 *  @param unique_term    The "unique" term.
	 *  @param document The new document.
	 *
	 *  @return         The document ID that document was given.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while writing to the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 */
	Xapian::docid replace_document(const std::string & unique_term,
				       const Xapian::Document & document);

	/** Add a word to the spelling dictionary.
	 *
	 *  If the word is already present, its frequency is increased.
	 *
	 *  @param word	    The word to add.
	 *  @param freqinc  How much to increase its frequency by (default 1).
	 */
	void add_spelling(const std::string & word,
			  Xapian::termcount freqinc = 1) const;

	/** Remove a word from the spelling dictionary.
	 *
	 *  The word's frequency is decreased, and if would become zero or less
	 *  then the word is removed completely.
	 *
	 *  @param word	    The word to remove.
	 *  @param freqdec  How much to decrease its frequency by (default 1).
	 */
	void remove_spelling(const std::string & word,
			     Xapian::termcount freqdec = 1) const;

	/** Add a synonym for a term.
	 *
	 *  If @a synonym is already a synonym for @a term, then no action is
	 *  taken.
	 */
	void add_synonym(const std::string & term,
			 const std::string & synonym) const;

	/** Remove a synonym for a term.
	 *
	 *  If @a synonym isn't a synonym for @a term, then no action is taken.
	 */
	void remove_synonym(const std::string & term,
			    const std::string & synonym) const;

	/** Remove all synonyms for a term.
	 *
	 *  If @a term has no synonyms, no action is taken.
	 */
	void clear_synonyms(const std::string & term) const;

	/** Set the user-specified metadata associated with a given key.
	 *
	 *  This method sets the metadata value associated with a given key.
	 *  If there is already a metadata value stored in the database with
	 *  the same key, the old value is replaced.  If you want to delete an
	 *  existing item of metadata, just set its value to the empty string.
	 *
	 *  User-specified metadata allows you to store arbitrary information
	 *  in the form of (key,tag) pairs.
	 *
	 *  There's no hard limit on the number of metadata items, or the size
	 *  of the metadata values.  Metadata keys have a limited length, which
	 *  depends on the backend.  We recommend limiting them to 200 bytes.
	 *  Empty keys are not valid, and specifying one will cause an
	 *  exception.
	 *
	 *  Metadata modifications are committed to disk in the same way as
	 *  modifications to the documents in the database are: i.e.,
	 *  modifications are atomic, and won't be committed to disk
	 *  immediately (see flush() for more details).  This allows metadata
	 *  to be used to link databases with versioned external resources
	 *  by storing the appropriate version number in a metadata item.
	 *
	 *  You can also use the metadata to store arbitrary extra information
	 *  associated with terms, documents, or postings by encoding the
	 *  termname and/or document id into the metadata key.
	 *
	 *  @param key   The key of the metadata item to set.
	 *
	 *  @param value The value of the metadata item to set.
	 *
	 *  @exception Xapian::DatabaseError will be thrown if a problem occurs
	 *             while writing to the database.
	 *
	 *  @exception Xapian::DatabaseCorruptError will be thrown if the
	 *             database is in a corrupt state.
	 *
	 *  @exception Xapian::InvalidArgumentError will be thrown if the
	 *             key supplied is empty.
	 */
	void set_metadata(const std::string & key, const std::string & value);

	/// Return a string describing this object.
	std::string get_description() const;
};

/** Open for read/write; create if no db exists. */
const int DB_CREATE_OR_OPEN = 1;
/** Create a new database; fail if db exists. */
const int DB_CREATE = 2;
/** Overwrite existing db; create if none exists. */
const int DB_CREATE_OR_OVERWRITE = 3;
/** Open for read/write; fail if no db exists. */
const int DB_OPEN = 4;
// Can't see any sensible use for this one
// const int DB_OVERWRITE = XXX;

}

#endif /* XAPIAN_INCLUDED_DATABASE_H */
